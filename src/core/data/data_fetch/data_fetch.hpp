#pragma once

#include <QJsonDocument>
#include <QJsonObject>
#include <QMutex>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QTimer>
#include <QUrlQuery>
#include <memory>

class DataFetch : public QObject
{
    Q_OBJECT

  public:
    // 单例访问方法
    static DataFetch *instance();

    // 禁用拷贝构造和赋值操作
    DataFetch(const DataFetch &) = delete;
    DataFetch &operator=(const DataFetch &) = delete;

    ~DataFetch();

    // 设置API基础URL
    void setBaseUrl(const QString &url);

    // Bilibili 模块
    QString bilibili_getUserInfo(const QString &mid = "");

    // Weather 模块
    QString weather_getCurrentWeather(const QString &location, const QString &lang = "", const QString &unit = "m");

    QString weather_getDailyForecast(int days, const QString &location, const QString &lang = "", const QString &unit = "m");

    QString weather_searchCity(const QString &location, const QString &adm = "", const QString &range = "", int number = 10, const QString &lang = "");

    // Clash 模块
    QString clash_getAllProxies();
    QString clash_getProxyInfo(const QString &proxyName);
    QString clash_selectProxy(const QString &proxyName, const QString &targetProxyName);

    // System 实时数据模块
    void startSystemRealtimeStream();
    void stopSystemRealtimeStream();
    bool isSystemRealtimeStreamActive() const;
    void enableAutoReconnect(bool enabled = true);
    void setReconnectInterval(int intervalMs = 5000);

  signals:
    void requestFinished(const QString &requestId, const QJsonObject &response);
    void requestError(const QString &requestId, const QString &error);
    void systemRealtimeDataReceived(const QJsonObject &data);
    void systemRealtimeStreamError(const QString &error);
    void systemRealtimeStreamStarted();
    void systemRealtimeStreamStopped();
    void systemRealtimeConnectionStatusChanged(const QString &status); // 新增：连接状态变化
    void systemRealtimeStreamReconnecting();

  private slots:
    void onRequestFinished(QNetworkReply *reply);
    void onRequestTimeout();
    void onSseDataReceived();
    void onSseError(QNetworkReply::NetworkError error);
    void onSseFinished();
    void onReconnectTimer();

  private:
    // 私有构造函数
    explicit DataFetch(QObject *parent = nullptr);

    QString makeRequest(const QString &method, const QString &path, const QUrlQuery &query = QUrlQuery(), const QJsonObject &body = QJsonObject());

    QString generateRequestId();
    void scheduleReconnect(const QString &reason);

    QNetworkAccessManager *m_networkManager;
    QString m_baseUrl;
    QMap<QNetworkReply *, QString> m_pendingRequests;
    QMap<QString, QTimer *> m_requestTimers;

    // SSE相关成员
    QNetworkReply *m_sseReply;
    bool m_sseStreamActive;
    QString m_sseBuffer; // 用于缓存不完整的数据行
    bool m_connectionEstablished; // 跟踪是否真正建立了连接（接收到数据）
    
    // 重连机制相关
    QTimer *m_reconnectTimer;
    bool m_autoReconnectEnabled;
    int m_reconnectIntervalMs;
    int m_reconnectAttempts;
    static constexpr int MAX_RECONNECT_ATTEMPTS = 10; // 最大重连尝试次数，-1表示无限重连

    // 单例实例
    static DataFetch *s_instance;
    static QMutex s_mutex;

    static constexpr int REQUEST_TIMEOUT = 30000; // 30秒超时
};