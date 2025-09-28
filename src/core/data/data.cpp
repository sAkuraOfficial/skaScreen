#include "./data.hpp"
#include "./data_fetch/data_fetch.hpp"
#include <QCoreApplication>
#include <QDebug>
#include <QObject>

// 同步版本（为了向后兼容，但会阻塞线程）
weather_now weather_now_get(QString location)
{
    // 警告：这是同步版本，会阻塞线程，建议使用异步版本
    qWarning() << "Using synchronous weather_now_get - consider using async version";

    weather_now result;
    DataFetch *fetch = DataFetch::instance();

    // 连接信号和槽来等待响应
    QObject context;
    bool finished = false;

    QObject::connect(fetch, &DataFetch::requestFinished, &context, [&](const QString &requestId, const QJsonObject &json) {
        QJsonObject data = json["data"].toObject();

        result.obsTime = data["obsTime"].toString();
        result.temp = data["temp"].toString();
        result.feelsLike = data["feelsLike"].toString();
        result.icon = data["icon"].toString();
        result.text = data["text"].toString();
        result.wind360 = data["wind360"].toString();
        result.windDir = data["windDir"].toString();
        result.windScale = data["windScale"].toString();
        result.windSpeed = data["windSpeed"].toString();
        result.humidity = data["humidity"].toString();
        result.precip = data["precip"].toString();
        result.pressure = data["pressure"].toString();
        result.vis = data["vis"].toString();
        result.cloud = data["cloud"].toString();
        result.dew = data["dew"].toString();
        result.updateTime = data["updateTime"].toString();
        result.fxLink = data["fxLink"].toString();

        finished = true;
    });

    QObject::connect(fetch, &DataFetch::requestError, &context, [&](const QString &requestId, const QString &error) {
        qDebug() << "Request error:" << error;
        finished = true;
    });

    QString requestId = fetch->weather_getCurrentWeather(location);

    // 等待完成（这会阻塞）
    while (!finished)
    {
        QCoreApplication::processEvents();
    }

    return result;
}

// 获取每日天气预报
std::vector<weather_daily> weather_daily_get(QString location, int days)
{
    // 警告：这是同步版本，会阻塞线程，建议使用异步版本
    qWarning() << "Using synchronous weather_daily_get - consider using async version";

    std::vector<weather_daily> result;
    DataFetch *fetch = DataFetch::instance();

    QObject context;
    bool finished = false;

    QObject::connect(fetch, &DataFetch::requestFinished, &context, [&](const QString &requestId, const QJsonObject &json) {
        QJsonArray dataArray = json["data"].toArray();
        for (const QJsonValue &value : dataArray)
        {
            QJsonObject data = value.toObject();
            weather_daily item;
            item.fxDate = data["fxDate"].toString();
            item.sunrise = data["sunrise"].toString();
            item.sunset = data["sunset"].toString();
            item.moonrise = data["moonrise"].toString();
            item.moonset = data["moonset"].toString();
            item.moonPhase = data["moonPhase"].toString();
            item.moonPhaseIcon = data["moonPhaseIcon"].toString();
            item.tempMax = data["tempMax"].toString();
            item.tempMin = data["tempMin"].toString();
            item.iconDay = data["iconDay"].toString();
            item.textDay = data["textDay"].toString();
            item.iconNight = data["iconNight"].toString();
            item.textNight = data["textNight"].toString();
            item.wind360Day = data["wind360Day"].toString();
            item.windDirDay = data["windDirDay"].toString();
            item.windScaleDay = data["windScaleDay"].toString();
            item.windSpeedDay = data["windSpeedDay"].toString();
            item.wind360Night = data["wind360Night"].toString();
            item.windDirNight = data["windDirNight"].toString();
            item.windScaleNight = data["windScaleNight"].toString();
            item.windSpeedNight = data["windSpeedNight"].toString();
            item.humidity = data["humidity"].toString();
            item.precip = data["precip"].toString();
            item.pressure = data["pressure"].toString();
            item.vis = data["vis"].toString();
            item.cloud = data["cloud"].toString();
            item.uvIndex = data["uvIndex"].toString();
            item.updateTime = data["updateTime"].toString();
            item.fxLink = data["fxLink"].toString();
            result.push_back(item);
        }
        finished = true;
    });

    QObject::connect(fetch, &DataFetch::requestError, &context, [&](const QString &requestId, const QString &error) {
        qDebug() << "Request error:" << error;
        finished = true;
    });

    QString requestId = fetch->weather_getDailyForecast(days, location);

    // 等待完成
    while (!finished)
    {
        QCoreApplication::processEvents();
    }

    return result;
}

// 获取城市搜索结果
std::vector<weather_city_search> weather_city_search_get(QString location)
{
    // 警告：这是同步版本，会阻塞线程，建议使用异步版本
    qWarning() << "Using synchronous weather_city_search_get - consider using async version";

    std::vector<weather_city_search> result;
    DataFetch *fetch = DataFetch::instance();

    QObject context;
    bool finished = false;

    QObject::connect(fetch, &DataFetch::requestFinished, &context, [&](const QString &requestId, const QJsonObject &json) {
        QJsonArray dataArray = json["data"].toArray();
        for (const QJsonValue &value : dataArray)
        {
            QJsonObject data = value.toObject();
            weather_city_search item;
            item.name = data["name"].toString();
            item.id = data["id"].toString();
            item.lat = data["lat"].toString();
            item.lon = data["lon"].toString();
            item.adm2 = data["adm2"].toString();
            item.adm1 = data["adm1"].toString();
            item.country = data["country"].toString();
            item.tz = data["tz"].toString();
            item.utcOffset = data["utcOffset"].toString();
            item.isDst = data["isDst"].toString() == "1";
            item.type = data["type"].toString();
            item.rank = data["rank"].toString();
            item.fxLink = data["fxLink"].toString();
            result.push_back(item);
        }
        finished = true;
    });

    QObject::connect(fetch, &DataFetch::requestError, &context, [&](const QString &requestId, const QString &error) {
        qDebug() << "Request error:" << error;
        finished = true;
    });

    QString requestId = fetch->weather_searchCity(location);

    // 等待完成
    while (!finished)
    {
        QCoreApplication::processEvents();
    }

    return result;
}

// 异步版本的函数实现
void weather_now_get_async(QString location, std::function<void(weather_now)> callback)
{
    DataFetch *fetch = DataFetch::instance();

    // 创建临时对象来管理连接
    QObject *context = new QObject();

    // 先发起请求获取请求ID
    QString targetRequestId = fetch->weather_getCurrentWeather(location);
    qDebug() << "weather_now_get_async: targetRequestId =" << targetRequestId;

    if (targetRequestId.isEmpty())
    {
        qDebug() << "Failed to create request";
        callback(weather_now{});
        context->deleteLater();
        return;
    }

    QObject::connect(fetch, &DataFetch::requestFinished, context, [callback, context, targetRequestId](const QString &requestId, const QJsonObject &json) {
        qDebug() << "requestFinished received: requestId =" << requestId << "targetRequestId =" << targetRequestId;

        // 检查是否是我们的请求
        if (requestId != targetRequestId)
        {
            qDebug() << "Request ID mismatch, ignoring";
            return; // 不是我们的请求，忽略
        }

        qDebug() << "Processing weather data, JSON:" << json;

        weather_now result;
        QJsonObject data = json["data"].toObject();

        result.obsTime = data["obsTime"].toString();
        result.temp = data["temp"].toString();
        result.feelsLike = data["feelsLike"].toString();
        result.icon = data["icon"].toString();
        result.text = data["text"].toString();
        result.wind360 = data["wind360"].toString();
        result.windDir = data["windDir"].toString();
        result.windScale = data["windScale"].toString();
        result.windSpeed = data["windSpeed"].toString();
        result.humidity = data["humidity"].toString();
        result.precip = data["precip"].toString();
        result.pressure = data["pressure"].toString();
        result.vis = data["vis"].toString();
        result.cloud = data["cloud"].toString();
        result.dew = data["dew"].toString();
        result.updateTime = data["updateTime"].toString();
        result.fxLink = data["fxLink"].toString();

        qDebug() << "Async weather data received:" << result.toString();
        callback(result);
        context->deleteLater();
    });

    QObject::connect(fetch, &DataFetch::requestError, context, [callback, context, targetRequestId](const QString &requestId, const QString &error) {
        qDebug() << "requestError received: requestId =" << requestId << "targetRequestId =" << targetRequestId;

        // 检查是否是我们的请求
        if (requestId != targetRequestId)
        {
            return; // 不是我们的请求，忽略
        }

        qDebug() << "Request error:" << error;
        // 返回空对象
        callback(weather_now{});
        context->deleteLater();
    });
}

void weather_daily_get_async(QString location, int days, std::function<void(std::vector<weather_daily>)> callback)
{
    DataFetch *fetch = DataFetch::instance();

    QObject *context = new QObject();

    // 先发起请求获取请求ID
    QString targetRequestId = fetch->weather_getDailyForecast(days, location);

    if (targetRequestId.isEmpty())
    {
        qDebug() << "Failed to create request";
        callback(std::vector<weather_daily>{});
        context->deleteLater();
        return;
    }

    QObject::connect(fetch, &DataFetch::requestFinished, context, [callback, context, targetRequestId](const QString &requestId, const QJsonObject &json) {
        // 检查是否是我们的请求
        if (requestId != targetRequestId)
        {
            return; // 不是我们的请求，忽略
        }

        std::vector<weather_daily> result;
        QJsonArray dataArray = json["data"].toArray();

        for (const QJsonValue &value : dataArray)
        {
            QJsonObject data = value.toObject();
            weather_daily item;
            item.fxDate = data["fxDate"].toString();
            item.sunrise = data["sunrise"].toString();
            item.sunset = data["sunset"].toString();
            item.moonrise = data["moonrise"].toString();
            item.moonset = data["moonset"].toString();
            item.moonPhase = data["moonPhase"].toString();
            item.moonPhaseIcon = data["moonPhaseIcon"].toString();
            item.tempMax = data["tempMax"].toString();
            item.tempMin = data["tempMin"].toString();
            item.iconDay = data["iconDay"].toString();
            item.textDay = data["textDay"].toString();
            item.iconNight = data["iconNight"].toString();
            item.textNight = data["textNight"].toString();
            item.wind360Day = data["wind360Day"].toString();
            item.windDirDay = data["windDirDay"].toString();
            item.windScaleDay = data["windScaleDay"].toString();
            item.windSpeedDay = data["windSpeedDay"].toString();
            item.wind360Night = data["wind360Night"].toString();
            item.windDirNight = data["windDirNight"].toString();
            item.windScaleNight = data["windScaleNight"].toString();
            item.windSpeedNight = data["windSpeedNight"].toString();
            item.humidity = data["humidity"].toString();
            item.precip = data["precip"].toString();
            item.pressure = data["pressure"].toString();
            item.vis = data["vis"].toString();
            item.cloud = data["cloud"].toString();
            item.uvIndex = data["uvIndex"].toString();
            item.updateTime = data["updateTime"].toString();
            item.fxLink = data["fxLink"].toString();
            result.push_back(item);
        }

        callback(result);
        context->deleteLater();
    });

    QObject::connect(fetch, &DataFetch::requestError, context, [callback, context, targetRequestId](const QString &requestId, const QString &error) {
        // 检查是否是我们的请求
        if (requestId != targetRequestId)
        {
            return; // 不是我们的请求，忽略
        }

        qDebug() << "Request error:" << error;
        callback(std::vector<weather_daily>{});
        context->deleteLater();
    });
}

void weather_city_search_get_async(QString location, std::function<void(std::vector<weather_city_search>)> callback)
{
    DataFetch *fetch = DataFetch::instance();

    QObject *context = new QObject();

    // 先发起请求获取请求ID
    QString targetRequestId = fetch->weather_searchCity(location);

    if (targetRequestId.isEmpty())
    {
        qDebug() << "Failed to create request";
        callback(std::vector<weather_city_search>{});
        context->deleteLater();
        return;
    }

    QObject::connect(fetch, &DataFetch::requestFinished, context, [callback, context, targetRequestId](const QString &requestId, const QJsonObject &json) {
        // 检查是否是我们的请求
        if (requestId != targetRequestId)
        {
            return; // 不是我们的请求，忽略
        }

        std::vector<weather_city_search> result;
        QJsonArray dataArray = json["data"].toArray();

        for (const QJsonValue &value : dataArray)
        {
            QJsonObject data = value.toObject();
            weather_city_search item;
            item.name = data["name"].toString();
            item.id = data["id"].toString();
            item.lat = data["lat"].toString();
            item.lon = data["lon"].toString();
            item.adm2 = data["adm2"].toString();
            item.adm1 = data["adm1"].toString();
            item.country = data["country"].toString();
            item.tz = data["tz"].toString();
            item.utcOffset = data["utcOffset"].toString();
            item.isDst = data["isDst"].toString() == "1";
            item.type = data["type"].toString();
            item.rank = data["rank"].toString();
            item.fxLink = data["fxLink"].toString();
            result.push_back(item);
        }

        callback(result);
        context->deleteLater();
    });

    QObject::connect(fetch, &DataFetch::requestError, context, [callback, context, targetRequestId](const QString &requestId, const QString &error) {
        // 检查是否是我们的请求
        if (requestId != targetRequestId)
        {
            return; // 不是我们的请求，忽略
        }

        qDebug() << "Request error:" << error;
        callback(std::vector<weather_city_search>{});
        context->deleteLater();
    });
}

// =============================================================================
// Bilibili 模块实现
// =============================================================================

// 同步版本：获取哔哩哔哩用户粉丝数
bilibili_user_follower bilibili_user_follower_get(QString uid)
{
    qWarning() << "Using synchronous bilibili_user_follower_get - consider using async version";

    bilibili_user_follower result;
    DataFetch *fetch = DataFetch::instance();

    QObject context;
    bool finished = false;

    QObject::connect(fetch, &DataFetch::requestFinished, &context, [&](const QString &requestId, const QJsonObject &json) {
        // 解析响应数据
        if (json.contains("data") && json["data"].isDouble())
        {
            result.follower_count = json["data"].toInt();
        }
        else
        {
            qDebug() << "Invalid bilibili response format";
            result.follower_count = 0;
        }
        finished = true;
    });

    QObject::connect(fetch, &DataFetch::requestError, &context, [&](const QString &requestId, const QString &error) {
        qDebug() << "Bilibili request error:" << error;
        result.follower_count = 0;
        finished = true;
    });

    QString requestId = fetch->bilibili_getUserInfo(uid);

    // 等待完成
    while (!finished)
    {
        QCoreApplication::processEvents();
    }

    return result;
}

// 异步版本：获取哔哩哔哩用户粉丝数
void bilibili_user_follower_get_async(QString uid, std::function<void(bilibili_user_follower)> callback)
{
    DataFetch *fetch = DataFetch::instance();

    QObject *context = new QObject();

    QString targetRequestId = fetch->bilibili_getUserInfo(uid);
    qDebug() << "bilibili_user_follower_get_async: targetRequestId =" << targetRequestId;

    if (targetRequestId.isEmpty())
    {
        qDebug() << "Failed to create bilibili request";
        callback(bilibili_user_follower{});
        context->deleteLater();
        return;
    }

    QObject::connect(fetch, &DataFetch::requestFinished, context, [callback, context, targetRequestId](const QString &requestId, const QJsonObject &json) {
        qDebug() << "bilibili requestFinished received: requestId =" << requestId << "targetRequestId =" << targetRequestId;

        if (requestId != targetRequestId)
        {
            return;
        }

        qDebug() << "Processing bilibili data, JSON:" << json;

        bilibili_user_follower result;
        if (json.contains("data") && json["data"].isDouble())
        {
            result.follower_count = json["data"].toInt();
        }
        else
        {
            qDebug() << "Invalid bilibili response format";
            result.follower_count = 0;
        }

        qDebug() << "Async bilibili data received:" << result.toString();
        callback(result);
        context->deleteLater();
    });

    QObject::connect(fetch, &DataFetch::requestError, context, [callback, context, targetRequestId](const QString &requestId, const QString &error) {
        qDebug() << "bilibili requestError received: requestId =" << requestId << "targetRequestId =" << targetRequestId;

        if (requestId != targetRequestId)
        {
            return;
        }

        qDebug() << "Bilibili request error:" << error;
        bilibili_user_follower result;
        result.follower_count = 0;
        callback(result);
        context->deleteLater();
    });
}

// =============================================================================
// Clash 模块实现
// =============================================================================

// 解析单个代理信息的辅助函数
clash_proxy_info parseProxyInfo(const QJsonObject &proxyData)
{
    clash_proxy_info info;

    info.alive = proxyData["alive"].toBool();
    info.dialerProxy = proxyData["dialerProxy"].toString();
    info.hidden = proxyData["hidden"].toBool();
    info.networkInterface = proxyData["networkInterface"].toString();
    info.mptcp = proxyData["mptcp"].toBool();
    info.name = proxyData["name"].toString();
    info.now = proxyData["now"].toString();
    info.routingMark = proxyData["routingMark"].toInt();
    info.smux = proxyData["smux"].toBool();
    info.tfo = proxyData["tfo"].toBool();
    info.type = proxyData["type"].toString();
    info.udp = proxyData["udp"].toBool();
    info.uot = proxyData["uot"].toBool();
    info.xudp = proxyData["xudp"].toBool();
    info.expectedStatus = proxyData["expectedStatus"].toString();
    info.fixed = proxyData["fixed"].toString();
    info.testUrl = proxyData["testUrl"].toString();

    // 解析all数组
    QJsonArray allArray = proxyData["all"].toArray();
    for (const QJsonValue &value : allArray)
    {
        info.all.push_back(value.toString());
    }

    // 解析history数组
    QJsonArray historyArray = proxyData["history"].toArray();
    for (const QJsonValue &value : historyArray)
    {
        info.history.push_back(value.toString());
    }

    return info;
}

// 解析TUN配置的辅助函数
clash_tun_config parseTunConfig(const QJsonObject &tunData)
{
    clash_tun_config config;

    config.enable = tunData["enable"].toBool();
    config.device = tunData["device"].toString();
    config.stack = tunData["stack"].toString();
    config.autoRoute = tunData["autoRoute"].toBool();
    config.autoDetectInterface = tunData["autoDetectInterface"].toBool();
    config.mtu = tunData["mtu"].toInt();

    // 解析DNS劫持数组
    QJsonArray dnsArray = tunData["dnsHijack"].toArray();
    for (const QJsonValue &value : dnsArray)
    {
        config.dnsHijack.push_back(value.toString());
    }

    // 解析IPv4地址数组
    QJsonArray addrArray = tunData["inet4Address"].toArray();
    for (const QJsonValue &value : addrArray)
    {
        config.inet4Address.push_back(value.toString());
    }

    return config;
}

// 解析Clash配置信息的辅助函数
clash_config_info parseClashConfig(const QJsonObject &configData)
{
    clash_config_info config;

    config.port = configData["port"].toInt();
    config.socksPort = configData["socksPort"].toInt();
    config.redirPort = configData["redirPort"].toInt();
    config.tproxyPort = configData["tproxyPort"].toInt();
    config.mixedPort = configData["mixedPort"].toInt();
    config.allowLan = configData["allowLan"].toBool();
    config.bindAddress = configData["bindAddress"].toString();
    config.mode = configData["mode"].toString();
    config.unifiedDelay = configData["unifiedDelay"].toBool();
    config.logLevel = configData["logLevel"].toString();
    config.ipv6 = configData["ipv6"].toBool();
    config.interfaceName = configData["interfaceName"].toString();
    config.routingMark = configData["routingMark"].toInt();
    config.geoAutoUpdate = configData["geoAutoUpdate"].toBool();
    config.geoUpdateInterval = configData["geoUpdateInterval"].toInt();
    config.geodataMode = configData["geodataMode"].toBool();
    config.geodataLoader = configData["geodataLoader"].toString();
    config.geositeMatcher = configData["geositeMatcher"].toString();
    config.tcpConcurrent = configData["tcpConcurrent"].toBool();
    config.findProcessMode = configData["findProcessMode"].toString();
    config.sniffing = configData["sniffing"].toBool();
    config.globalUa = configData["globalUa"].toString();
    config.etagSupport = configData["etagSupport"].toBool();

    // 解析TUN配置
    QJsonObject tunObj = configData["tun"].toObject();
    config.tun = parseTunConfig(tunObj);

    return config;
}

// 同步版本：获取所有代理列表
clash_proxy_list clash_get_all_proxies()
{
    qWarning() << "Using synchronous clash_get_all_proxies - consider using async version";

    clash_proxy_list result;
    DataFetch *fetch = DataFetch::instance();

    QObject context;
    bool finished = false;

    QObject::connect(fetch, &DataFetch::requestFinished, &context, [&](const QString &requestId, const QJsonObject &json) {
        QJsonObject dataObject = json["data"].toObject();

        // 遍历所有代理
        for (auto it = dataObject.begin(); it != dataObject.end(); ++it)
        {
            QString proxyName = it.key();
            QJsonObject proxyData = it.value().toObject();

            clash_proxy_info info = parseProxyInfo(proxyData);
            result.proxies[proxyName] = info;
        }

        finished = true;
    });

    QObject::connect(fetch, &DataFetch::requestError, &context, [&](const QString &requestId, const QString &error) {
        qDebug() << "Clash request error:" << error;
        finished = true;
    });

    QString requestId = fetch->clash_getAllProxies();

    // 等待完成
    while (!finished)
    {
        QCoreApplication::processEvents();
    }

    return result;
}

// 同步版本：获取指定代理信息
clash_proxy_info clash_get_proxy_info(QString proxyName)
{
    qWarning() << "Using synchronous clash_get_proxy_info - consider using async version";

    clash_proxy_info result;
    DataFetch *fetch = DataFetch::instance();

    QObject context;
    bool finished = false;

    QObject::connect(fetch, &DataFetch::requestFinished, &context, [&](const QString &requestId, const QJsonObject &json) {
        QJsonObject dataObject = json["data"].toObject();
        result = parseProxyInfo(dataObject);
        finished = true;
    });

    QObject::connect(fetch, &DataFetch::requestError, &context, [&](const QString &requestId, const QString &error) {
        qDebug() << "Clash request error:" << error;
        finished = true;
    });

    QString requestId = fetch->clash_getProxyInfo(proxyName);

    // 等待完成
    while (!finished)
    {
        QCoreApplication::processEvents();
    }

    return result;
}

// 同步版本：切换代理
clash_operation_result clash_select_proxy(QString proxyName, QString targetProxyName)
{
    qWarning() << "Using synchronous clash_select_proxy - consider using async version";

    clash_operation_result result;
    DataFetch *fetch = DataFetch::instance();

    QObject context;
    bool finished = false;

    QObject::connect(fetch, &DataFetch::requestFinished, &context, [&](const QString &requestId, const QJsonObject &json) {
        result.success = json["success"].toBool();
        result.message = json["message"].toString();
        finished = true;
    });

    QObject::connect(fetch, &DataFetch::requestError, &context, [&](const QString &requestId, const QString &error) {
        qDebug() << "Clash request error:" << error;
        result.success = false;
        result.message = error;
        finished = true;
    });

    QString requestId = fetch->clash_selectProxy(proxyName, targetProxyName);

    // 等待完成
    while (!finished)
    {
        QCoreApplication::processEvents();
    }

    return result;
}

// 同步版本：获取Clash配置信息
clash_config_info clash_get_config()
{
    qWarning() << "Using synchronous clash_get_config - consider using async version";

    clash_config_info result;
    DataFetch *fetch = DataFetch::instance();

    QObject context;
    bool finished = false;

    QObject::connect(fetch, &DataFetch::requestFinished, &context, [&](const QString &requestId, const QJsonObject &json) {
        QJsonObject data = json["data"].toObject();
        result = parseClashConfig(data);
        finished = true;
    });

    QObject::connect(fetch, &DataFetch::requestError, &context, [&](const QString &requestId, const QString &error) {
        qDebug() << "clash_get_config error:" << error;
        finished = true;
    });

    QString requestId = fetch->clash_getConfig();

    // 等待完成
    while (!finished)
    {
        QCoreApplication::processEvents();
    }

    return result;
}

// 同步版本：获取当前Clash代理模式
ClashProxyMode clash_get_current_mode()
{
    clash_config_info config = clash_get_config();
    return config.getProxyMode();
}

// 同步版本：设置Clash代理模式
clash_mode_operation_result clash_set_mode(ClashProxyMode mode)
{
    qWarning() << "Using synchronous clash_set_mode - consider using async version";

    clash_mode_operation_result result;
    result.mode = mode;
    DataFetch *fetch = DataFetch::instance();

    QObject context;
    bool finished = false;

    QObject::connect(fetch, &DataFetch::requestFinished, &context, [&](const QString &requestId, const QJsonObject &json) {
        result.success = (json["code"].toInt() == 0);
        result.message = json["message"].toString();
        if (result.message.isEmpty() && result.success)
        {
            result.message = "Mode set successfully";
        }
        finished = true;
    });

    QObject::connect(fetch, &DataFetch::requestError, &context, [&](const QString &requestId, const QString &error) {
        result.success = false;
        result.message = error;
        finished = true;
    });

    QString requestId = fetch->clash_setMode(clashProxyModeToString(mode));

    // 等待完成
    while (!finished)
    {
        QCoreApplication::processEvents();
    }

    return result;
}

// 异步版本：获取所有代理列表
void clash_get_all_proxies_async(std::function<void(clash_proxy_list)> callback)
{
    DataFetch *fetch = DataFetch::instance();

    QObject *context = new QObject();

    QString targetRequestId = fetch->clash_getAllProxies();
    qDebug() << "clash_get_all_proxies_async: targetRequestId =" << targetRequestId;

    if (targetRequestId.isEmpty())
    {
        qDebug() << "Failed to create clash request";
        callback(clash_proxy_list{});
        context->deleteLater();
        return;
    }

    QObject::connect(fetch, &DataFetch::requestFinished, context, [callback, context, targetRequestId](const QString &requestId, const QJsonObject &json) {
        qDebug() << "clash requestFinished received: requestId =" << requestId << "targetRequestId =" << targetRequestId;

        if (requestId != targetRequestId)
        {
            return;
        }

        qDebug() << "Processing clash proxy list data, JSON:" << json;

        clash_proxy_list result;
        QJsonObject dataObject = json["data"].toObject();

        // 遍历所有代理
        for (auto it = dataObject.begin(); it != dataObject.end(); ++it)
        {
            QString proxyName = it.key();
            QJsonObject proxyData = it.value().toObject();

            clash_proxy_info info = parseProxyInfo(proxyData);
            result.proxies[proxyName] = info;
        }

        qDebug() << "Async clash proxy list data received";
        callback(result);
        context->deleteLater();
    });

    QObject::connect(fetch, &DataFetch::requestError, context, [callback, context, targetRequestId](const QString &requestId, const QString &error) {
        qDebug() << "clash requestError received: requestId =" << requestId << "targetRequestId =" << targetRequestId;

        if (requestId != targetRequestId)
        {
            return;
        }

        qDebug() << "Clash request error:" << error;
        callback(clash_proxy_list{});
        context->deleteLater();
    });
}

// 异步版本：获取指定代理信息
void clash_get_proxy_info_async(QString proxyName, std::function<void(clash_proxy_info)> callback)
{
    DataFetch *fetch = DataFetch::instance();

    QObject *context = new QObject();

    QString targetRequestId = fetch->clash_getProxyInfo(proxyName);
    qDebug() << "clash_get_proxy_info_async: targetRequestId =" << targetRequestId;

    if (targetRequestId.isEmpty())
    {
        qDebug() << "Failed to create clash request";
        callback(clash_proxy_info{});
        context->deleteLater();
        return;
    }

    QObject::connect(fetch, &DataFetch::requestFinished, context, [callback, context, targetRequestId](const QString &requestId, const QJsonObject &json) {
        qDebug() << "clash requestFinished received: requestId =" << requestId << "targetRequestId =" << targetRequestId;

        if (requestId != targetRequestId)
        {
            return;
        }

        qDebug() << "Processing clash proxy info data, JSON:" << json;

        QJsonObject dataObject = json["data"].toObject();
        clash_proxy_info result = parseProxyInfo(dataObject);

        qDebug() << "Async clash proxy info data received:" << result.toString();
        callback(result);
        context->deleteLater();
    });

    QObject::connect(fetch, &DataFetch::requestError, context, [callback, context, targetRequestId](const QString &requestId, const QString &error) {
        qDebug() << "clash requestError received: requestId =" << requestId << "targetRequestId =" << targetRequestId;

        if (requestId != targetRequestId)
        {
            return;
        }

        qDebug() << "Clash request error:" << error;
        callback(clash_proxy_info{});
        context->deleteLater();
    });
}

// 异步版本：切换代理
void clash_select_proxy_async(QString proxyName, QString targetProxyName, std::function<void(clash_operation_result)> callback)
{
    DataFetch *fetch = DataFetch::instance();

    QObject *context = new QObject();

    QString targetRequestId = fetch->clash_selectProxy(proxyName, targetProxyName);
    qDebug() << "clash_select_proxy_async: targetRequestId =" << targetRequestId;

    if (targetRequestId.isEmpty())
    {
        qDebug() << "Failed to create clash request";
        clash_operation_result result;
        result.success = false;
        result.message = "Failed to create request";
        callback(result);
        context->deleteLater();
        return;
    }

    QObject::connect(fetch, &DataFetch::requestFinished, context, [callback, context, targetRequestId](const QString &requestId, const QJsonObject &json) {
        qDebug() << "clash requestFinished received: requestId =" << requestId << "targetRequestId =" << targetRequestId;

        if (requestId != targetRequestId)
        {
            return;
        }

        qDebug() << "Processing clash operation result, JSON:" << json;

        clash_operation_result result;
        result.success = json["success"].toBool();
        result.message = json["message"].toString();

        qDebug() << "Async clash operation result received:" << result.toString();
        callback(result);
        context->deleteLater();
    });

    QObject::connect(fetch, &DataFetch::requestError, context, [callback, context, targetRequestId](const QString &requestId, const QString &error) {
        qDebug() << "clash requestError received: requestId =" << requestId << "targetRequestId =" << targetRequestId;

        if (requestId != targetRequestId)
        {
            return;
        }

        qDebug() << "Clash request error:" << error;
        clash_operation_result result;
        result.success = false;
        result.message = error;
        callback(result);
        context->deleteLater();
    });
}

// 异步版本：获取Clash配置信息
void clash_get_config_async(std::function<void(clash_config_info)> callback)
{
    DataFetch *fetch = DataFetch::instance();

    QObject *context = new QObject();

    QString targetRequestId = fetch->clash_getConfig();
    qDebug() << "clash_get_config_async: targetRequestId =" << targetRequestId;

    if (targetRequestId.isEmpty())
    {
        clash_config_info result;
        callback(result);
        context->deleteLater();
        return;
    }

    QObject::connect(fetch, &DataFetch::requestFinished, context, [callback, context, targetRequestId](const QString &requestId, const QJsonObject &json) {
        if (requestId == targetRequestId)
        {
            clash_config_info result;
            QJsonObject data = json["data"].toObject();
            result = parseClashConfig(data);

            qDebug() << "clash_get_config_async response received:" << result.toString();
            callback(result);
            context->deleteLater();
        }
    });

    QObject::connect(fetch, &DataFetch::requestError, context, [callback, context, targetRequestId](const QString &requestId, const QString &error) {
        if (requestId == targetRequestId)
        {
            qDebug() << "clash_get_config_async error:" << error;
            clash_config_info result; // 返回空结果
            callback(result);
            context->deleteLater();
        }
    });
}

// 异步版本：获取当前Clash代理模式
void clash_get_current_mode_async(std::function<void(ClashProxyMode)> callback)
{
    clash_get_config_async([callback](clash_config_info config) {
        callback(config.getProxyMode());
    });
}

// 异步版本：设置Clash代理模式
void clash_set_mode_async(ClashProxyMode mode, std::function<void(clash_mode_operation_result)> callback)
{
    DataFetch *fetch = DataFetch::instance();

    QObject *context = new QObject();

    QString targetRequestId = fetch->clash_setMode(clashProxyModeToString(mode));
    qDebug() << "clash_set_mode_async: targetRequestId =" << targetRequestId;

    if (targetRequestId.isEmpty())
    {
        clash_mode_operation_result result;
        result.success = false;
        result.message = "Failed to create request";
        result.mode = mode;
        callback(result);
        context->deleteLater();
        return;
    }

    QObject::connect(fetch, &DataFetch::requestFinished, context, [callback, context, targetRequestId, mode](const QString &requestId, const QJsonObject &json) {
        if (requestId == targetRequestId)
        {
            clash_mode_operation_result result;
            result.success = (json["code"].toInt() == 0);
            result.message = json["message"].toString();
            result.mode = mode;

            if (result.message.isEmpty() && result.success)
            {
                result.message = "Mode set successfully";
            }

            qDebug() << "clash_set_mode_async response received:" << result.toString();
            callback(result);
            context->deleteLater();
        }
    });

    QObject::connect(fetch, &DataFetch::requestError, context, [callback, context, targetRequestId, mode](const QString &requestId, const QString &error) {
        if (requestId == targetRequestId)
        {
            qDebug() << "clash_set_mode_async error:" << error;
            clash_mode_operation_result result;
            result.success = false;
            result.message = error;
            result.mode = mode;
            callback(result);
            context->deleteLater();
        }
    });
}

// =============================================================================
// System 实时数据模块实现
// =============================================================================

// 用于存储回调函数的静态变量
static std::function<void(clash_traffic_info)> g_clashTrafficCallback = nullptr;
static QObject *g_clashTrafficContext = nullptr;
static std::function<void(system_realtime_info)> g_systemRealtimeCallback = nullptr;
static std::function<void(QString)> g_systemStatusCallback = nullptr;
static QObject *g_systemRealtimeContext = nullptr;

// Clash 流量实时数据流
void clash_traffic_start_stream(std::function<void(clash_traffic_info)> callback)
{
    DataFetch *fetch = DataFetch::instance();

    if (fetch->isClashTrafficStreamActive())
    {
        qDebug() << "Stopping existing clash traffic stream before starting new one";
        clash_traffic_stop_stream();
    }

    g_clashTrafficCallback = callback;

    if (g_clashTrafficContext)
    {
        g_clashTrafficContext->deleteLater();
    }
    g_clashTrafficContext = new QObject();

    QObject::connect(fetch, &DataFetch::clashTrafficDataReceived, g_clashTrafficContext, [](const QJsonObject &json) {
        if (!g_clashTrafficCallback)
        {
            return;
        }

        clash_traffic_info info;
        info.up = json.value("up").toVariant().toLongLong();
        info.down = json.value("down").toVariant().toLongLong();
        info.timestamp = json.value("timestamp").toVariant().toLongLong();

        g_clashTrafficCallback(info);
    });

    QObject::connect(fetch, &DataFetch::clashTrafficStreamError, g_clashTrafficContext, [](const QString &error) {
        qDebug() << "Clash traffic stream error:" << error;
    });

    QObject::connect(fetch, &DataFetch::clashTrafficStreamStopped, g_clashTrafficContext, []() {
        qDebug() << "Clash traffic stream stopped";
        if (g_clashTrafficContext)
        {
            g_clashTrafficContext->deleteLater();
            g_clashTrafficContext = nullptr;
        }
        g_clashTrafficCallback = nullptr;
    });

    fetch->startClashTrafficStream();

    qDebug() << "Clash traffic stream started";
}

void clash_traffic_stop_stream()
{
    DataFetch *fetch = DataFetch::instance();
    fetch->stopClashTrafficStream();

    if (g_clashTrafficContext)
    {
        g_clashTrafficContext->deleteLater();
        g_clashTrafficContext = nullptr;
    }

    g_clashTrafficCallback = nullptr;

    qDebug() << "Clash traffic stream stopped by user";
}

bool clash_traffic_is_stream_active()
{
    DataFetch *fetch = DataFetch::instance();
    return fetch->isClashTrafficStreamActive();
}

void clash_traffic_start_stream_async(std::function<void(clash_traffic_info)> callback)
{
    clash_traffic_start_stream(callback);
}

void clash_traffic_stop_stream_async()
{
    clash_traffic_stop_stream();
}

// 启动系统实时数据流
void system_realtime_start_stream(std::function<void(system_realtime_info)> callback)
{
    DataFetch *fetch = DataFetch::instance();

    // 如果已经有活跃的流，先停止它
    if (fetch->isSystemRealtimeStreamActive())
    {
        qDebug() << "Stopping existing realtime stream before starting new one";
        system_realtime_stop_stream();
    }

    // 存储回调函数
    g_systemRealtimeCallback = callback;

    // 创建上下文对象
    if (g_systemRealtimeContext)
    {
        g_systemRealtimeContext->deleteLater();
    }
    g_systemRealtimeContext = new QObject();

    // 连接SSE数据接收信号
    QObject::connect(fetch, &DataFetch::systemRealtimeDataReceived, g_systemRealtimeContext, [](const QJsonObject &json) {
        if (!g_systemRealtimeCallback)
        {
            return;
        }

        qDebug() << "Received system realtime data:" << json;

        // 解析JSON数据为system_realtime_info对象
        system_realtime_info info;
        info.cpuUsage = json["cpuUsage"].toString();
        info.cpuFrequency = json["cpuFrequency"].toString();
        info.memoryUsage = json["memoryUsage"].toString();
        info.totalMemory = json["totalMemory"].toString();
        info.usedMemory = json["usedMemory"].toString();
        info.timestamp = json["timestamp"].toVariant().toLongLong();

        qDebug() << "Parsed system realtime info:" << info.toString();

        // 调用回调函数
        g_systemRealtimeCallback(info);
    });

    // 连接错误信号
    QObject::connect(fetch, &DataFetch::systemRealtimeStreamError, g_systemRealtimeContext, [](const QString &error) {
        qDebug() << "System realtime stream error:" << error;
        // 可以选择在这里通知用户或重试
    });

    // 连接流停止信号
    QObject::connect(fetch, &DataFetch::systemRealtimeStreamStopped, g_systemRealtimeContext, []() {
        qDebug() << "System realtime stream stopped";
        if (g_systemRealtimeContext)
        {
            g_systemRealtimeContext->deleteLater();
            g_systemRealtimeContext = nullptr;
        }
        g_systemRealtimeCallback = nullptr;
    });

    // 启动SSE流
    fetch->startSystemRealtimeStream();

    qDebug() << "System realtime stream started";
}

// 启动系统实时数据流（带状态回调）
void system_realtime_start_stream_with_status(
    std::function<void(system_realtime_info)> dataCallback,
    std::function<void(QString)> statusCallback
)
{
    DataFetch *fetch = DataFetch::instance();

    // 如果已经有活跃的流，先停止它
    if (fetch->isSystemRealtimeStreamActive())
    {
        qDebug() << "Stopping existing realtime stream before starting new one";
        system_realtime_stop_stream();
    }

    // 存储回调函数
    g_systemRealtimeCallback = dataCallback;
    g_systemStatusCallback = statusCallback;

    // 创建上下文对象
    if (g_systemRealtimeContext)
    {
        g_systemRealtimeContext->deleteLater();
    }
    g_systemRealtimeContext = new QObject();

    // 连接SSE数据接收信号
    QObject::connect(fetch, &DataFetch::systemRealtimeDataReceived, g_systemRealtimeContext, [](const QJsonObject &json) {
        if (!g_systemRealtimeCallback)
        {
            return;
        }

        // 解析JSON数据为system_realtime_info对象
        system_realtime_info info;
        info.cpuUsage = json["cpuUsage"].toString();
        info.cpuFrequency = json["cpuFrequency"].toString();
        info.memoryUsage = json["memoryUsage"].toString();
        info.totalMemory = json["totalMemory"].toString();
        info.usedMemory = json["usedMemory"].toString();
        info.timestamp = json["timestamp"].toVariant().toLongLong();

        // 调用数据回调函数
        g_systemRealtimeCallback(info);
    });

    // 连接状态信号
    QObject::connect(fetch, &DataFetch::systemRealtimeStreamStarted, g_systemRealtimeContext, []() {
        if (g_systemStatusCallback)
        {
            g_systemStatusCallback("connected");
        }
    });

    QObject::connect(fetch, &DataFetch::systemRealtimeStreamReconnecting, g_systemRealtimeContext, []() {
        if (g_systemStatusCallback)
        {
            g_systemStatusCallback("reconnecting");
        }
    });

    // 新增：监听连接状态变化信号
    QObject::connect(fetch, &DataFetch::systemRealtimeConnectionStatusChanged, g_systemRealtimeContext, [](const QString &status) {
        if (g_systemStatusCallback)
        {
            qDebug() << "Connection status updated:" << status;
            g_systemStatusCallback(status);
        }
    });

    QObject::connect(fetch, &DataFetch::systemRealtimeStreamError, g_systemRealtimeContext, [](const QString &error) {
        if (g_systemStatusCallback)
        {
            g_systemStatusCallback("error: " + error);
        }
    });

    QObject::connect(fetch, &DataFetch::systemRealtimeStreamStopped, g_systemRealtimeContext, []() {
        if (g_systemStatusCallback)
        {
            g_systemStatusCallback("disconnected");
        }
        if (g_systemRealtimeContext)
        {
            g_systemRealtimeContext->deleteLater();
            g_systemRealtimeContext = nullptr;
        }
        g_systemRealtimeCallback = nullptr;
        g_systemStatusCallback = nullptr;
    });

    // 启动SSE流
    fetch->startSystemRealtimeStream();

    qDebug() << "System realtime stream with status callbacks started";
}

// 停止系统实时数据流
void system_realtime_stop_stream()
{
    DataFetch *fetch = DataFetch::instance();
    fetch->stopSystemRealtimeStream();

    // 清理回调和上下文
    if (g_systemRealtimeContext)
    {
        g_systemRealtimeContext->deleteLater();
        g_systemRealtimeContext = nullptr;
    }
    g_systemRealtimeCallback = nullptr;

    qDebug() << "System realtime stream stopped by user";
}

// 检查系统实时数据流是否活跃
bool system_realtime_is_stream_active()
{
    DataFetch *fetch = DataFetch::instance();
    return fetch->isSystemRealtimeStreamActive();
}