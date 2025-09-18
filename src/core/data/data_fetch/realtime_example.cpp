#include "../data.hpp"
#include "../data_fetch/data_fetch.hpp"
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>

// 系统实时数据使用示例
void example_system_realtime_usage()
{
    qDebug() << "=== 系统实时数据使用示例 ===";
    
    // 启动实时数据流
    system_realtime_start_stream([](system_realtime_info info) {
        qDebug() << "收到实时硬件数据:";
        qDebug() << "  CPU使用率:" << info.cpuUsage;
        qDebug() << "  CPU频率:" << info.cpuFrequency;
        qDebug() << "  内存使用率:" << info.memoryUsage;
        qDebug() << "  总内存:" << info.totalMemory;
        qDebug() << "  已用内存:" << info.usedMemory;
        qDebug() << "  时间戳:" << info.timestamp;
        qDebug() << "---";
    });
    
    qDebug() << "实时数据流已启动，流状态:" << (system_realtime_is_stream_active() ? "活跃" : "非活跃");
    
    // 10秒后停止流
    QTimer::singleShot(10000, []() {
        qDebug() << "10秒后，停止实时数据流...";
        system_realtime_stop_stream();
        qDebug() << "实时数据流已停止，流状态:" << (system_realtime_is_stream_active() ? "活跃" : "非活跃");
        
        // 退出应用程序
        QCoreApplication::quit();
    });
}

// 哔哩哔哩数据使用示例
void example_bilibili_usage()
{
    qDebug() << "=== 哔哩哔哩数据使用示例 ===";
    
    // 异步获取用户粉丝数
    bilibili_user_follower_get_async("358071887", [](bilibili_user_follower follower) {
        qDebug() << "异步获取哔哩哔哩用户粉丝数:" << follower.toString();
    });
}

// Clash代理使用示例  
void example_clash_usage()
{
    qDebug() << "=== Clash代理使用示例 ===";
    
    // 异步获取所有代理列表
    clash_get_all_proxies_async([](clash_proxy_list proxies) {
        qDebug() << "异步获取所有代理列表:";
        for (const auto& pair : proxies.proxies) {
            qDebug() << "代理名称:" << pair.first;
            qDebug() << "代理信息:" << pair.second.toString();
            qDebug() << "---";
        }
    });
    
    // 异步获取指定代理信息
    clash_get_proxy_info_async("🇭🇰 香港节点", [](clash_proxy_info info) {
        qDebug() << "异步获取指定代理信息:" << info.toString();
    });
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    qDebug() << "开始数据模块使用示例...";
    
    // 确保服务器运行在 http://localhost:8080
    DataFetch::instance()->setBaseUrl("http://localhost:8080");
    
    // 运行示例
    example_system_realtime_usage();
    example_bilibili_usage();
    example_clash_usage();
    
    return app.exec();
}