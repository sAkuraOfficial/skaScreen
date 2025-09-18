#include "data_fetch.hpp"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QUrlQuery>
#include <QDebug>
#include <QTimer>
#include <QUuid>
#include <qeventloop.h>
#include <QMutexLocker>

// 静态成员定义
DataFetch* DataFetch::s_instance = nullptr;
QMutex DataFetch::s_mutex;

DataFetch* DataFetch::instance()
{
    // 双重检查锁定模式确保线程安全
    if (s_instance == nullptr) {
        QMutexLocker locker(&s_mutex);
        if (s_instance == nullptr) {
            s_instance = new DataFetch();
        }
    }
    return s_instance;
}

DataFetch::DataFetch(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_baseUrl("http://localhost:8080")
    , m_sseReply(nullptr)
    , m_sseStreamActive(false)
    , m_connectionEstablished(false)
    , m_reconnectTimer(new QTimer(this))
    , m_autoReconnectEnabled(true)
    , m_reconnectIntervalMs(5000)
    , m_reconnectAttempts(0)
{
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &DataFetch::onRequestFinished);
    
    // 设置重连定时器
    m_reconnectTimer->setSingleShot(true);
    connect(m_reconnectTimer, &QTimer::timeout, this, &DataFetch::onReconnectTimer);
}

DataFetch::~DataFetch()
{
    // 停止SSE流和重连定时器
    stopSystemRealtimeStream();
    if (m_reconnectTimer) {
        m_reconnectTimer->stop();
    }
    
    // 清理待处理的请求和定时器
    for (auto timer : m_requestTimers) {
        timer->stop();
        timer->deleteLater();
    }
    m_requestTimers.clear();
    m_pendingRequests.clear();
}

void DataFetch::setBaseUrl(const QString &url)
{
    m_baseUrl = url;
}

QString DataFetch::generateRequestId()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QString DataFetch::makeRequest(const QString &method, const QString &path, 
                               const QUrlQuery &query, const QJsonObject &body)
{
    QUrl url(m_baseUrl + path);
    if (!query.isEmpty()) {
        url.setQuery(query);
    }

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("User-Agent", "skaScreen/1.0");

    QNetworkReply *reply = nullptr;
    
    if (method == "GET") {
        reply = m_networkManager->get(request);
    } else if (method == "PUT") {
        QJsonDocument doc(body);
        reply = m_networkManager->put(request, doc.toJson());
    } else if (method == "POST") {
        QJsonDocument doc(body);
        reply = m_networkManager->post(request, doc.toJson());
    }

    if (reply) {
        QString requestId = generateRequestId();
        m_pendingRequests[reply] = requestId;
        
        // 设置超时定时器
        QTimer *timer = new QTimer(this);
        timer->setSingleShot(true);
        timer->setInterval(REQUEST_TIMEOUT);
        connect(timer, &QTimer::timeout, [this, reply]() {
            onRequestTimeout();
        });
        m_requestTimers[requestId] = timer;
        timer->start();
        
        qDebug() << "Making" << method << "request to:" << url.toString();
        return requestId;
    }
    
    return QString();
}

void DataFetch::onRequestFinished(QNetworkReply *reply)
{
    if (!reply) {
        qDebug() << "onRequestFinished: reply is null";
        return;
    }

    QString requestId = m_pendingRequests.take(reply);
    qDebug() << "onRequestFinished: requestId =" << requestId;
    
    if (m_requestTimers.contains(requestId)) {
        m_requestTimers[requestId]->stop();
        m_requestTimers[requestId]->deleteLater();
        m_requestTimers.remove(requestId);
    }

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        qDebug() << "Response data:" << data;
        QJsonDocument doc = QJsonDocument::fromJson(data);
        qDebug() << "Emitting requestFinished signal with requestId:" << requestId;
        emit requestFinished(requestId, doc.object());
    } else {
        qDebug() << "Network error:" << reply->errorString();
        emit requestError(requestId, reply->errorString());
    }

    reply->deleteLater();
}

void DataFetch::onRequestTimeout()
{
    QTimer *timer = qobject_cast<QTimer*>(sender());
    if (!timer) return;

    QString requestId;
    for (auto it = m_requestTimers.begin(); it != m_requestTimers.end(); ++it) {
        if (it.value() == timer) {
            requestId = it.key();
            break;
        }
    }

    if (!requestId.isEmpty()) {
        // 查找对应的reply并取消
        for (auto it = m_pendingRequests.begin(); it != m_pendingRequests.end(); ++it) {
            if (it.value() == requestId) {
                it.key()->abort();
                break;
            }
        }
        
        emit requestError(requestId, "Request timeout");
        m_requestTimers.remove(requestId);
    }
    
    timer->deleteLater();
}

// Bilibili 模块实现
QString DataFetch::bilibili_getUserInfo(const QString &mid)
{
    QUrlQuery query;
    if (!mid.isEmpty()) {
        query.addQueryItem("uid", mid);
    }
    
    // return makeRequest("GET", "/api/bilibili/user/info", query);
    return makeRequest("GET", "/api/bilibili/user/follower", query);
}

// Weather 模块实现
QString DataFetch::weather_getCurrentWeather(const QString &location, 
                                             const QString &lang, 
                                             const QString &unit)
{
    QUrlQuery query;
    query.addQueryItem("location", location);
    if (!lang.isEmpty()) {
        query.addQueryItem("lang", lang);
    }
    if (!unit.isEmpty()) {
        query.addQueryItem("unit", unit);
    }
    
    return makeRequest("GET", "/api/weather/now", query);
}

QString DataFetch::weather_getDailyForecast(int days, 
                                            const QString &location, 
                                            const QString &lang, 
                                            const QString &unit)
{
    QUrlQuery query;
    query.addQueryItem("location", location);
    if (!lang.isEmpty()) {
        query.addQueryItem("lang", lang);
    }
    if (!unit.isEmpty()) {
        query.addQueryItem("unit", unit);
    }
    
    QString path = QString("/api/weather/%1").arg(days);
    return makeRequest("GET", path, query);
}

QString DataFetch::weather_searchCity(const QString &location, 
                                      const QString &adm, 
                                      const QString &range, 
                                      int number, 
                                      const QString &lang)
{
    QUrlQuery query;
    query.addQueryItem("location", location);
    if (!adm.isEmpty()) {
        query.addQueryItem("adm", adm);
    }
    if (!range.isEmpty()) {
        query.addQueryItem("range", range);
    }
    if (number != 10) {
        query.addQueryItem("number", QString::number(number));
    }
    if (!lang.isEmpty()) {
        query.addQueryItem("lang", lang);
    }
    
    return makeRequest("GET", "/api/weather/city/search", query);
}

// Clash 模块实现
QString DataFetch::clash_getAllProxies()
{
    return makeRequest("GET", "/api/clash/proxies");
}

QString DataFetch::clash_getProxyInfo(const QString &proxyName)
{
    QString path = QString("/api/clash/proxies/%1").arg(QString::fromUtf8(QUrl::toPercentEncoding(proxyName)));
    return makeRequest("GET", path);
}

QString DataFetch::clash_selectProxy(const QString &proxyName, const QString &targetProxyName)
{
    QUrlQuery query;
    query.addQueryItem("targetProxyName", targetProxyName);
    
    QString path = QString("/api/clash/proxies/%1").arg(QString::fromUtf8(QUrl::toPercentEncoding(proxyName)));
    return makeRequest("PUT", path, query);
}

// =============================================================================
// System 实时数据模块实现
// =============================================================================

void DataFetch::startSystemRealtimeStream()
{
    if (m_sseStreamActive) {
        qDebug() << "SSE stream is already active";
        return;
    }
    
    // 停止重连定时器（如果正在运行）
    if (m_reconnectTimer->isActive()) {
        m_reconnectTimer->stop();
    }
    
    qDebug() << "Starting system realtime SSE stream (attempt" << (m_reconnectAttempts + 1) << ")";
    
    // 构建SSE请求
    QUrl url(m_baseUrl + "/api/system/realtime/stream");
    QNetworkRequest request(url);
    
    // 设置SSE请求头
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/event-stream");
    request.setRawHeader("Accept", "text/event-stream");
    request.setRawHeader("Cache-Control", "no-cache");
    
    // 发起GET请求
    m_sseReply = m_networkManager->get(request);
    
    if (m_sseReply) {
        m_sseStreamActive = true;
        m_sseBuffer.clear();
        m_connectionEstablished = false; // 重要：只有接收到数据才算真正连接
        
        // 连接信号槽
        connect(m_sseReply, &QNetworkReply::readyRead,
                this, &DataFetch::onSseDataReceived);
        connect(m_sseReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
                this, &DataFetch::onSseError);
        connect(m_sseReply, &QNetworkReply::finished,
                this, &DataFetch::onSseFinished);
        
        // 不要立即发出连接成功信号，等待真正接收到数据
        qDebug() << "SSE request created, waiting for data...";
    } else {
        qDebug() << "Failed to start SSE stream";
        scheduleReconnect("Failed to create SSE request");
    }
}

void DataFetch::stopSystemRealtimeStream()
{
    // 停止重连定时器
    if (m_reconnectTimer && m_reconnectTimer->isActive()) {
        m_reconnectTimer->stop();
        qDebug() << "Stopped reconnect timer";
    }
    
    if (!m_sseStreamActive || !m_sseReply) {
        // 即使流不活跃，也要重置状态
        m_sseStreamActive = false;
        m_reconnectAttempts = 0;
        return;
    }
    
    qDebug() << "Stopping system realtime SSE stream";
    
    m_sseStreamActive = false;
    m_connectionEstablished = false; // 重置连接状态
    m_reconnectAttempts = 0; // 重置重连计数器
    
    // 断开连接并删除reply
    if (m_sseReply) {
        m_sseReply->disconnect();
        m_sseReply->abort();
        m_sseReply->deleteLater();
        m_sseReply = nullptr;
    }
    
    m_sseBuffer.clear();
    emit systemRealtimeConnectionStatusChanged("disconnected");
    emit systemRealtimeStreamStopped();
    
    qDebug() << "SSE stream stopped";
}

bool DataFetch::isSystemRealtimeStreamActive() const
{
    return m_sseStreamActive;
}

void DataFetch::onSseDataReceived()
{
    if (!m_sseReply) {
        return;
    }
    
    // 如果这是第一次接收到数据，表示连接真正建立
    if (!m_connectionEstablished) {
        m_connectionEstablished = true;
        m_reconnectAttempts = 0; // 重置重连计数器
        emit systemRealtimeStreamStarted();
        emit systemRealtimeConnectionStatusChanged("connected");
        qDebug() << "SSE connection established successfully";
    }
    
    // 读取所有可用数据
    QByteArray data = m_sseReply->readAll();
    QString text = QString::fromUtf8(data);
    
    // 将新数据添加到缓冲区
    m_sseBuffer += text;
    
    // 按行处理数据
    QStringList lines = m_sseBuffer.split('\n');
    
    // 保留最后一行（可能不完整）
    if (!m_sseBuffer.endsWith('\n')) {
        m_sseBuffer = lines.takeLast();
    } else {
        m_sseBuffer.clear();
    }
    
    // 处理每一行
    for (const QString& line : lines) {
        QString trimmedLine = line.trimmed();
        
        if (trimmedLine.isEmpty()) {
            // 空行，跳过
            continue;
        }
        
        // 处理SSE数据行
        if (trimmedLine.startsWith("data:")) {
            QString jsonStr = trimmedLine.mid(5).trimmed(); // 去掉"data:"前缀
            
            if (!jsonStr.isEmpty()) {
                // 解析JSON数据
                QJsonParseError error;
                QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &error);
                
                if (error.error == QJsonParseError::NoError) {
                    QJsonObject obj = doc.object();
                    qDebug() << "Received SSE data:" << obj;
                    emit systemRealtimeDataReceived(obj);
                } else {
                    qDebug() << "Failed to parse SSE JSON data:" << error.errorString();
                    qDebug() << "Raw data:" << jsonStr;
                }
            }
        }
        // 忽略其他SSE事件类型（如事件类型、ID等）
    }
}

void DataFetch::onSseError(QNetworkReply::NetworkError error)
{
    qDebug() << "SSE stream error:" << error;
    
    QString errorString = "Unknown error";
    if (m_sseReply) {
        errorString = m_sseReply->errorString();
    }
    
    // 清理当前连接
    m_sseStreamActive = false;
    m_connectionEstablished = false; // 重置连接状态
    if (m_sseReply) {
        m_sseReply->disconnect();
        m_sseReply->deleteLater();
        m_sseReply = nullptr;
    }
    
    // 安排重连
    scheduleReconnect(errorString);
}

void DataFetch::onSseFinished()
{
    qDebug() << "SSE stream finished";
    
    QString errorString;
    bool hasError = false;
    
    if (m_sseReply && m_sseReply->error() != QNetworkReply::NoError) {
        errorString = m_sseReply->errorString();
        hasError = true;
    }
    
    // 清理当前连接
    m_sseStreamActive = false;
    m_connectionEstablished = false; // 重置连接状态
    if (m_sseReply) {
        m_sseReply->disconnect();
        m_sseReply->deleteLater();
        m_sseReply = nullptr;
    }
    
    if (hasError) {
        // 安排重连
        scheduleReconnect(errorString);
    } else {
        // 正常关闭，不重连
        emit systemRealtimeStreamStopped();
    }
}

// =============================================================================
// 重连机制实现
// =============================================================================

void DataFetch::enableAutoReconnect(bool enabled)
{
    m_autoReconnectEnabled = enabled;
    qDebug() << "Auto reconnect" << (enabled ? "enabled" : "disabled");
}

void DataFetch::setReconnectInterval(int intervalMs)
{
    m_reconnectIntervalMs = intervalMs;
    qDebug() << "Reconnect interval set to" << intervalMs << "ms";
}

void DataFetch::scheduleReconnect(const QString &reason)
{
    if (!m_autoReconnectEnabled) {
        qDebug() << "Auto reconnect disabled, not scheduling reconnect";
        emit systemRealtimeStreamError(reason);
        emit systemRealtimeStreamStopped();
        return;
    }
    
    m_reconnectAttempts++;
    
    if (MAX_RECONNECT_ATTEMPTS > 0 && m_reconnectAttempts > MAX_RECONNECT_ATTEMPTS) {
        qDebug() << "Max reconnect attempts reached (" << MAX_RECONNECT_ATTEMPTS << "), giving up";
        emit systemRealtimeStreamError("Max reconnect attempts reached: " + reason);
        emit systemRealtimeStreamStopped();
        m_reconnectAttempts = 0;
        return;
    }
    
    qDebug() << "Scheduling reconnect in" << m_reconnectIntervalMs << "ms (attempt" << m_reconnectAttempts << "reason:" << reason << ")";
    
    emit systemRealtimeStreamReconnecting();
    emit systemRealtimeConnectionStatusChanged("reconnecting");
    emit systemRealtimeStreamError("Disconnected, reconnecting in " + QString::number(m_reconnectIntervalMs / 1000) + " seconds: " + reason);
    
    // 启动重连定时器
    if (m_reconnectTimer) {
        m_reconnectTimer->start(m_reconnectIntervalMs);
    }
}

void DataFetch::onReconnectTimer()
{
    qDebug() << "Reconnect timer triggered, attempting to reconnect...";
    
    // 确保之前的连接已清理
    if (m_sseReply) {
        m_sseReply->disconnect();
        m_sseReply->deleteLater();
        m_sseReply = nullptr;
    }
    
    m_sseStreamActive = false;
    m_sseBuffer.clear();
    
    // 尝试重新连接
    startSystemRealtimeStream();
}