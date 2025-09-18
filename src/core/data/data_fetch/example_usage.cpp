// 使用示例文件 - 展示如何使用 DataFetch API 模块（单例模式）
#include "data_fetch.hpp"
#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument>

void exampleUsage()
{
    // 获取 DataFetch 单例实例
    DataFetch *dataFetch = DataFetch::instance();
    
    // 可选：设置不同的API服务器地址
    // dataFetch->setBaseUrl("http://your-api-server:8080");
    
    // 1. 获取 Bilibili 用户信息
    qDebug() << "=== Bilibili 用户信息 ===";
    QJsonObject biliUserInfo = dataFetch->bilibili_getUserInfo("468991");
    qDebug() << "Response:" << QJsonDocument(biliUserInfo).toJson(QJsonDocument::Compact);
    
    // 2. 获取当前天气
    qDebug() << "\n=== 当前天气 ===";
    QJsonObject currentWeather = dataFetch->weather_getCurrentWeather("101281601");
    qDebug() << "Response:" << QJsonDocument(currentWeather).toJson(QJsonDocument::Compact);
    
    // 3. 获取7天天气预报
    qDebug() << "\n=== 7天天气预报 ===";
    QJsonObject forecast = dataFetch->weather_getDailyForecast(7, "101010100");
    qDebug() << "Response:" << QJsonDocument(forecast).toJson(QJsonDocument::Compact);
    
    // 4. 搜索城市
    qDebug() << "\n=== 城市搜索 ===";
    QJsonObject citySearch = dataFetch->weather_searchCity("东莞");
    qDebug() << "Response:" << QJsonDocument(citySearch).toJson(QJsonDocument::Compact);
    
    // 5. 获取所有代理信息
    qDebug() << "\n=== Clash 代理信息 ===";
    QJsonObject allProxies = dataFetch->clash_getAllProxies();
    qDebug() << "Response:" << QJsonDocument(allProxies).toJson(QJsonDocument::Compact);
    
    // 6. 获取特定代理信息
    qDebug() << "\n=== 特定代理信息 ===";
    QJsonObject proxyInfo = dataFetch->clash_getProxyInfo("🇭🇰 香港节点");
    qDebug() << "Response:" << QJsonDocument(proxyInfo).toJson(QJsonDocument::Compact);
    
    // 7. 选择代理
    qDebug() << "\n=== 选择代理 ===";
    QJsonObject selectResult = dataFetch->clash_selectProxy("🚀 节点选择", "🇭🇰 香港节点");
    qDebug() << "Response:" << QJsonDocument(selectResult).toJson(QJsonDocument::Compact);
    
    // 注意：单例模式下不需要手动删除实例
}

// 异步使用示例
void asyncExampleUsage()
{
    DataFetch *dataFetch = DataFetch::instance();
    
    // 连接信号槽来处理异步响应
    QObject::connect(dataFetch, &DataFetch::requestFinished, 
                     [](const QString &requestId, const QJsonObject &response) {
        qDebug() << "Request" << requestId << "finished with response:";
        qDebug() << QJsonDocument(response).toJson(QJsonDocument::Compact);
    });
    
    QObject::connect(dataFetch, &DataFetch::requestError, 
                     [](const QString &requestId, const QString &error) {
        qDebug() << "Request" << requestId << "failed with error:" << error;
    });
    
    // 发起异步请求
    // 注意：当前实现是同步的，如需异步版本需要修改 makeRequest 方法
}

// 展示在不同地方使用单例
void useInAnotherFunction()
{
    // 在任何地方都可以直接获取单例实例
    DataFetch *api = DataFetch::instance();
    
    // 检查网络连接或进行简单的API调用
    QJsonObject result = api->bilibili_getUserInfo("123456");
    
    if (result.contains("code") && result["code"].toInt() == 0) {
        qDebug() << "API调用成功";
    } else {
        qDebug() << "API调用失败:" << result["message"].toString();
    }
}
