#include "src/core/data_fetch/data_fetch.hpp"
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    // 简单测试
    DataFetch dataFetch;
    qDebug() << "DataFetch 模块初始化成功";
    
    // 测试 API 调用（需要确保服务器运行在 localhost:8080）
    /*
    QJsonObject result = dataFetch.bilibili_getUserInfo("468991");
    qDebug() << "Bilibili API 测试结果:" << result;
    */
    
    return 0;
}
