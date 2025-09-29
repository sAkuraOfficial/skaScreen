#pragma once
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QMetaType>
#include <QObject>
#include <functional>
#include <map>
#include <qstring.h>
#include <string>
#include <vector>
class weather_now
{
  public:
    QString obsTime;    // 观测时间（如 "2025-09-10T15:00+08:00"）
    QString temp;       // 当前温度（摄氏度）
    QString feelsLike;  // 体感温度（摄氏度）
    QString icon;       // 天气图标代码
    QString text;       // 天气状况描述（如“阴”）
    QString wind360;    // 风向360角度
    QString windDir;    // 风向（如“南风”）
    QString windScale;  // 风力等级
    QString windSpeed;  // 风速（公里/小时）
    QString humidity;   // 相对湿度（百分比）
    QString precip;     // 降水量（毫米）
    QString pressure;   // 大气压强（百帕）
    QString vis;        // 能见度（公里）
    QString cloud;      // 云量（百分比）
    QString dew;        // 露点温度（摄氏度）
    QString updateTime; // 数据更新时间（如 "2025-09-10T15:02+08:00"）
    QString fxLink;     // 详情链接

    QString toString() const
    {
        return "obsTime: " + obsTime +
               ", temp: " + temp +
               ", feelsLike: " + feelsLike +
               ", icon: " + icon +
               ", text: " + text +
               ", wind360: " + wind360 +
               ", windDir: " + windDir +
               ", windScale: " + windScale +
               ", windSpeed: " + windSpeed +
               ", humidity: " + humidity +
               ", precip: " + precip +
               ", pressure: " + pressure +
               ", vis: " + vis +
               ", cloud: " + cloud +
               ", dew: " + dew +
               ", updateTime: " + updateTime +
               ", fxLink: " + fxLink;
    }
};

class weather_daily
{
  public:
    QString fxDate;         // 预报日期（如 "2025-09-10"）
    QString sunrise;        // 日出时间
    QString sunset;         // 日落时间
    QString moonrise;       // 月升时间
    QString moonset;        // 月落时间
    QString moonPhase;      // 月相名称
    QString moonPhaseIcon;  // 月相图标代码
    QString tempMax;        // 最高温度（摄氏度）
    QString tempMin;        // 最低温度（摄氏度）
    QString iconDay;        // 白天天气图标代码
    QString textDay;        // 白天天气状况描述
    QString iconNight;      // 夜间天气图标代码
    QString textNight;      // 夜间天气状况描述
    QString wind360Day;     // 白天风向360角度
    QString windDirDay;     // 白天风向
    QString windScaleDay;   // 白天风力等级（如“1-3”）
    QString windSpeedDay;   // 白天风速（公里/小时）
    QString wind360Night;   // 夜间风向360角度
    QString windDirNight;   // 夜间风向
    QString windScaleNight; // 夜间风力等级（如“1-3”）
    QString windSpeedNight; // 夜间风速（公里/小时）
    QString humidity;       // 相对湿度（百分比）
    QString precip;         // 降水量（毫米）
    QString pressure;       // 大气压强（百帕）
    QString vis;            // 能见度（公里）
    QString cloud;          // 云量（百分比）
    QString uvIndex;        // 紫外线指数
    QString updateTime;     // 数据更新时间
    QString fxLink;         // 详情链接

    QString toString() const
    {
        return "fxDate: " + fxDate +
               ", sunrise: " + sunrise +
               ", sunset: " + sunset +
               ", moonrise: " + moonrise +
               ", moonset: " + moonset +
               ", moonPhase: " + moonPhase +
               ", moonPhaseIcon: " + moonPhaseIcon +
               ", tempMax: " + tempMax +
               ", tempMin: " + tempMin +
               ", iconDay: " + iconDay +
               ", textDay: " + textDay +
               ", iconNight: " + iconNight +
               ", textNight: " + textNight +
               ", wind360Day: " + wind360Day +
               ", windDirDay: " + windDirDay +
               ", windScaleDay: " + windScaleDay +
               ", windSpeedDay: " + windSpeedDay +
               ", wind360Night: " + wind360Night +
               ", windDirNight: " + windDirNight +
               ", windScaleNight: " + windScaleNight +
               ", windSpeedNight: " + windSpeedNight +
               ", humidity: " + humidity +
               ", precip: " + precip +
               ", pressure: " + pressure +
               ", vis: " + vis +
               ", cloud: " + cloud +
               ", uvIndex: " + uvIndex +
               ", updateTime: " + updateTime +
               ", fxLink: " + fxLink;
    }
};

class weather_city_search
{
  public:
    QString name;      // 城市名称
    QString id;        // 城市ID
    QString lat;       // 纬度
    QString lon;       // 经度
    QString adm2;      // 二级行政区
    QString adm1;      // 一级行政区
    QString country;   // 国家
    QString tz;        // 时区
    QString utcOffset; // UTC偏移
    bool isDst;        // 是否夏令时（0/1）
    QString type;      // 类型（如"city"）
    QString rank;      // 排名
    QString fxLink;    // 详情链接

    QString toString() const
    {
        return "name: " + name +
               ", id: " + id +
               ", lat: " + lat +
               ", lon: " + lon +
               ", adm2: " + adm2 +
               ", adm1: " + adm1 +
               ", country: " + country +
               ", tz: " + tz +
               ", utcOffset: " + utcOffset +
               ", isDst: " + (isDst ? "true" : "false") +
               ", type: " + type +
               ", rank: " + rank +
               ", fxLink: " + fxLink;
    }
};

// 系统实时硬件信息数据类
class system_realtime_info
{
  public:
    QString cpuUsage;     // CPU使用率（如"11.8%"）
    QString cpuFrequency; // CPU频率（如"1.75 GHz"）
    QString memoryUsage;  // 内存使用率（如"57.9%"）
    QString totalMemory;  // 总内存（如"39.80 GB"）
    QString usedMemory;   // 已用内存（如"23.06 GB"）
    qint64 timestamp;     // 时间戳

    QString toString() const
    {
        return "cpuUsage: " + cpuUsage +
               ", cpuFrequency: " + cpuFrequency +
               ", memoryUsage: " + memoryUsage +
               ", totalMemory: " + totalMemory +
               ", usedMemory: " + usedMemory +
               ", timestamp: " + QString::number(timestamp);
    }
};

// 哔哩哔哩用户粉丝数据类
class bilibili_user_follower
{
  public:
    int follower_count; // 粉丝数量

    QString toString() const
    {
        return "follower_count: " + QString::number(follower_count);
    }
};

// Clash 代理信息类
class clash_proxy_info
{
  public:
    bool alive;                   // 代理是否存活
    std::vector<QString> all;     // 所有可用代理列表
    QString dialerProxy;          // 拨号代理
    bool hidden;                  // 是否隐藏
    std::vector<QString> history; // 历史记录
    QString icon;                 // 图标
    QString networkInterface;     // 网络接口
    bool mptcp;                   // 是否支持MPTCP
    QString name;                 // 代理名称
    QString now;                  // 当前选择的代理
    int routingMark;              // 路由标记
    bool smux;                    // 是否支持SMUX
    bool tfo;                     // 是否支持TCP Fast Open
    QString type;                 // 代理类型 (Selector, URLTest等)
    bool udp;                     // 是否支持UDP
    bool uot;                     // 是否支持UoT
    bool xudp;                    // 是否支持XUDP
    QString expectedStatus;       // 期望状态
    QString fixed;                // 固定设置
    QString testUrl;              // 测试URL

    QString toString() const
    {
        QString allStr = "[";
        for (size_t i = 0; i < all.size(); ++i)
        {
            allStr += all[i];
            if (i < all.size() - 1)
                allStr += ", ";
        }
        allStr += "]";

        return QString("alive: ") + QString(alive ? "true" : "false") +
               ", all: " + allStr +
               ", dialerProxy: " + dialerProxy +
               ", hidden: " + QString(hidden ? "true" : "false") +
               ", name: " + name +
               ", now: " + now +
               ", routingMark: " + QString::number(routingMark) +
               ", type: " + type +
               ", udp: " + QString(udp ? "true" : "false") +
               ", expectedStatus: " + expectedStatus +
               ", testUrl: " + testUrl;
    }
};

// Clash 代理列表类 (用于存储多个代理信息)
class clash_proxy_list
{
  public:
    std::map<QString, clash_proxy_info> proxies; // 代理名称到代理信息的映射

    QString toString() const
    {
        QString result = "Proxies:\n";
        for (const auto &pair : proxies)
        {
            result += "  " + pair.first + ": " + pair.second.toString() + "\n";
        }
        return result;
    }
};

// Clash 代理模式枚举
enum class ClashProxyMode
{
    Rule,   // rule 模式
    Global, // global 模式
    Direct  // direct 模式
};

// 将枚举转换为字符串
inline QString clashProxyModeToString(ClashProxyMode mode)
{
    switch (mode)
    {
    case ClashProxyMode::Rule:
        return "rule";
    case ClashProxyMode::Global:
        return "global";
    case ClashProxyMode::Direct:
        return "direct";
    default:
        return "rule";
    }
}

// 从字符串转换为枚举
inline ClashProxyMode stringToClashProxyMode(const QString &str)
{
    if (str == "global")
        return ClashProxyMode::Global;
    if (str == "direct")
        return ClashProxyMode::Direct;
    return ClashProxyMode::Rule;
}

// Clash TUN 配置类
class clash_tun_config
{
  public:
    bool enable;                       // 是否启用
    QString device;                    // 设备名称
    QString stack;                     // 网络栈
    std::vector<QString> dnsHijack;    // DNS劫持
    bool autoRoute;                    // 自动路由
    bool autoDetectInterface;          // 自动检测网络接口
    int mtu;                           // MTU
    std::vector<QString> inet4Address; // IPv4地址

    QString toString() const
    {
        QString dnsStr = "[";
        for (size_t i = 0; i < dnsHijack.size(); ++i)
        {
            dnsStr += dnsHijack[i];
            if (i < dnsHijack.size() - 1)
                dnsStr += ", ";
        }
        dnsStr += "]";

        QString addrStr = "[";
        for (size_t i = 0; i < inet4Address.size(); ++i)
        {
            addrStr += inet4Address[i];
            if (i < inet4Address.size() - 1)
                addrStr += ", ";
        }
        addrStr += "]";

        return "enable: " + QString(enable ? "true" : "false") +
               ", device: " + device +
               ", stack: " + stack +
               ", dnsHijack: " + dnsStr +
               ", autoRoute: " + QString(autoRoute ? "true" : "false") +
               ", autoDetectInterface: " + QString(autoDetectInterface ? "true" : "false") +
               ", mtu: " + QString::number(mtu) +
               ", inet4Address: " + addrStr;
    }
};

// Clash 配置信息类
class clash_config_info
{
  public:
    int port;                // HTTP代理端口
    int socksPort;           // SOCKS5代理端口
    int redirPort;           // 透明代理重定向端口
    int tproxyPort;          // TProxy端口
    int mixedPort;           // 混合端口
    clash_tun_config tun;    // TUN配置
    bool allowLan;           // 是否允许局域网连接
    QString bindAddress;     // 绑定地址
    QString mode;            // 代理模式
    bool unifiedDelay;       // 统一延迟
    QString logLevel;        // 日志级别
    bool ipv6;               // 是否支持IPv6
    QString interfaceName;   // 网络接口名称
    int routingMark;         // 路由标记
    bool geoAutoUpdate;      // 地理位置数据自动更新
    int geoUpdateInterval;   // 地理位置数据更新间隔（小时）
    bool geodataMode;        // 地理位置数据模式
    QString geodataLoader;   // 地理位置数据加载器
    QString geositeMatcher;  // Geosite匹配器
    bool tcpConcurrent;      // TCP并发
    QString findProcessMode; // 查找进程模式
    bool sniffing;           // 嗅探
    QString globalUa;        // 全局用户代理
    bool etagSupport;        // ETag支持

    ClashProxyMode getProxyMode() const
    {
        return stringToClashProxyMode(mode);
    }

    QString toString() const
    {
        return "port: " + QString::number(port) +
               ", socksPort: " + QString::number(socksPort) +
               ", redirPort: " + QString::number(redirPort) +
               ", tproxyPort: " + QString::number(tproxyPort) +
               ", mixedPort: " + QString::number(mixedPort) +
               ", allowLan: " + QString(allowLan ? "true" : "false") +
               ", bindAddress: " + bindAddress +
               ", mode: " + mode +
               ", unifiedDelay: " + QString(unifiedDelay ? "true" : "false") +
               ", logLevel: " + logLevel +
               ", ipv6: " + QString(ipv6 ? "true" : "false") +
               ", interfaceName: " + interfaceName +
               ", routingMark: " + QString::number(routingMark) +
               ", tun: {" + tun.toString() + "}" +
               ", geoAutoUpdate: " + QString(geoAutoUpdate ? "true" : "false") +
               ", geoUpdateInterval: " + QString::number(geoUpdateInterval) +
               ", geodataMode: " + QString(geodataMode ? "true" : "false") +
               ", geodataLoader: " + geodataLoader +
               ", geositeMatcher: " + geositeMatcher +
               ", tcpConcurrent: " + QString(tcpConcurrent ? "true" : "false") +
               ", findProcessMode: " + findProcessMode +
               ", sniffing: " + QString(sniffing ? "true" : "false") +
               ", globalUa: " + globalUa +
               ", etagSupport: " + QString(etagSupport ? "true" : "false");
    }
};

// Clash 模式操作结果类
class clash_mode_operation_result
{
  public:
    bool success;        // 操作是否成功
    QString message;     // 返回消息
    ClashProxyMode mode; // 设置的模式

    QString toString() const
    {
        return QString("success: ") + QString(success ? "true" : "false") +
               ", message: " + message +
               ", mode: " + clashProxyModeToString(mode);
    }
};

// Clash 操作结果类
class clash_operation_result
{
  public:
    bool success;    // 操作是否成功
    QString message; // 返回消息

    QString toString() const
    {
        return QString("success: ") + QString(success ? "true" : "false") +
               ", message: " + message;
    }
};

// Clash 流量信息类
class clash_traffic_info
{
  public:
    qint64 up = 0;        // 上行速率（kbps）
    qint64 down = 0;      // 下行速率（kbps）
    qint64 timestamp = 0; // 时间戳

    QString toString() const
    {
        return QString("up: %1 kbps, down: %2 kbps, timestamp: %3")
            .arg(up)
            .arg(down)
            .arg(timestamp);
    }
};


// Bilibili 模块函数
bilibili_user_follower bilibili_user_follower_get(QString uid);

// Clash 模块函数
void clash_traffic_start_stream(std::function<void(clash_traffic_info)> callback);
void clash_traffic_stop_stream();
bool clash_traffic_is_stream_active();

// System 实时数据模块函数
void system_realtime_start_stream(std::function<void(system_realtime_info)> callback);
void system_realtime_start_stream_with_status(
    std::function<void(system_realtime_info)> dataCallback,
    std::function<void(QString)> statusCallback
);
void system_realtime_stop_stream();
bool system_realtime_is_stream_active();

// 异步版本的天气函数
void weather_city_search_get_async(QString location, std::function<void(std::vector<weather_city_search>)> callback);
void weather_daily_get_async(QString location, int days, std::function<void(std::vector<weather_daily>)> callback);
void weather_now_get_async(QString location, std::function<void(weather_now)> callback);

// 异步版本的Bilibili函数
void bilibili_user_follower_get_async(QString uid, std::function<void(bilibili_user_follower)> callback);

// 异步版本的Clash函数
void clash_get_all_proxies_async(std::function<void(clash_proxy_list)> callback);
void clash_get_proxy_info_async(QString proxyName, std::function<void(clash_proxy_info)> callback);
void clash_select_proxy_async(QString proxyName, QString targetProxyName, std::function<void(clash_operation_result)> callback);
void clash_get_config_async(std::function<void(clash_config_info)> callback);
void clash_get_current_mode_async(std::function<void(ClashProxyMode)> callback);
void clash_set_mode_async(ClashProxyMode mode, std::function<void(clash_mode_operation_result)> callback);
void clash_traffic_start_stream_async(std::function<void(clash_traffic_info)> callback);
void clash_traffic_stop_stream_async();

Q_DECLARE_METATYPE(clash_traffic_info)
