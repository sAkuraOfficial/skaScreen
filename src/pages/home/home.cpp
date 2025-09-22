#include "home.h"
#include "../../core/data/data.hpp"
#include "../../core/data/data_fetch/data_fetch.hpp"
#include "../../utils/blur.h"
#include "../../utils/img.h"
#include <QLocale>
#include <QToolTip>
#include <QTimer>
#include <qstyleoption.h>
#include <tyme.h>
#include<qgraphicslayout.h>
home::home(QWidget *parent)
    : QWidget(parent)

      ,
      m_connectionStatusLabel(nullptr)
{
    ui.setupUi(this);

    // 注册自定义类型以便在信号槽中使用
    qRegisterMetaType<system_realtime_info>("system_realtime_info");

    // 覆盖一下ui文件设置的样式表。
    setStyleSheet(QString::fromUtf8("#widget,#widget_2,#widget_3,#widget_4,#widget_system_info,#widget_5{\n"
                                    "	border-radius:8;\n"
                                    "background-color: rgba(222, 204, 214,160);\n"
                                    "}"));

    // QVector<BlurEffect *> effects;
    // for (int i = 0; i <= 4; i++)
    // {
    //     auto *blur = new BlurEffect();
    //     blur->set_blur_radius(16); // 可调整模糊半径
    //     blur->set_border_radius(8);
    //     effects.append(blur);
    // }

    // // 设置到widget上
    // ui.widget->setGraphicsEffect(effects[0]);
    // ui.widget_2->setGraphicsEffect(effects[1]);
    // ui.widget_3->setGraphicsEffect(effects[2]);
    // ui.widget_4->setGraphicsEffect(effects[3]);
    // ui.widget_5->setGraphicsEffect(effects[4]);

    // setAttribute(Qt::WA_TranslucentBackground, true);
    // ui.widget->setAttribute(Qt::WA_TranslucentBackground, true);
    // ui.widget_2->setAttribute(Qt::WA_TranslucentBackground, true);
    // ui.widget_3->setAttribute(Qt::WA_TranslucentBackground, true);
    // ui.widget_4->setAttribute(Qt::WA_TranslucentBackground, true);
    // ui.widget_5->setAttribute(Qt::WA_TranslucentBackground, true);

    // 使用方法
    QPixmap pix(":/images/test/tx.jpg");
    QPixmap roundPix = getRoundPixmap(pix);
    ui.label_blbl_tx_show_img->setPixmap(roundPix);

    QPixmap src(":/images/test/music.jpg");
    int radius = qMin(src.width(), src.height()) * 0.12; // 12% 圆角
    QPixmap rounded = getRoundPixmap_radius(src, radius);
    ui.music_img->setPixmap(rounded);

    // 更新天气
    timer_30min = new QTimer(this);
    connect(timer_30min, &QTimer::timeout, this, &home::updateCurrentWeather_30min);
    timer_30min->start(1800000);  // 30分钟更新一次天气
    updateCurrentWeather_30min(); // 启动时立即更新一次

    // 更新时间
    timer_1s = new QTimer(this);
    connect(timer_1s, &QTimer::timeout, this, &home::updateCurrentTime_1s);
    timer_1s->start(1000); // 1秒更新一次时间

    // 更新粉丝数量
    timer_1min = new QTimer(this);
    connect(timer_1min, &QTimer::timeout, this, &home::updateCurrentBilibiliFans_1min);
    timer_1min->start(1000 * 60);     // 一分钟
    updateCurrentBilibiliFans_1min(); // 启动的时候立刻更新一次

    // 初始化系统监控UI和数据流
    qDebug() << "开始初始化系统监控UI...";
    if (initSystemInfoUI())
    {
        qDebug() << "系统监控UI初始化成功，启动监控...";
        startSystemMonitoring();
    }
    else
    {
        qDebug() << "Failed to initialize system monitoring UI";
    }
}

home::~home()
{
    stopSystemMonitoring();
}

void home::updateCurrentWeather_30min()
{
    weather_now_get_async("101281601", [this](weather_now result) {
        // 在UI线程中更新界面
        if (!result.text.isEmpty())
        {
            qDebug() << result.toString();
            ui.label_template_show->setText(result.temp + "°C");
            // m_descriptionLabel->setText(QString::fromStdString(result.text));
            // m_humidityLabel->setText(QString::number(result.humidity) + "%");
        }
        else
        {
            ui.label_template_show->setText("获取失败");
        }
    });
}

void home::updateCurrentTime_1s()
{
    using namespace tyme;

    // 更新当前时间
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString currentTimeString = currentDateTime.toString("hh:mm:ss");
    ui.label_time_show->setText(currentTimeString);

    // 更新农历
    //  先用qt获取当前年月日
    auto date = currentDateTime.date();
    auto solar_day = SolarDay::from_ymd(date.year(), date.month(), date.day());
    auto lunar_day_str = solar_day.get_lunar_day().to_string();
    // 去除前面的"农历"，精准匹配,不要使用substr
    std::string lunar_prefix = "农历";
    if (lunar_day_str.substr(0, lunar_prefix.length()) == lunar_prefix)
    {
        lunar_day_str.erase(0, lunar_prefix.length());
    }

    ui.label_moon_date_show->setText(QString::fromStdString(lunar_day_str));

    int dayOfWeek = date.dayOfWeek(); // 1=周一, 7=周日

    // 获取今天的星期几中文名称
    QString weekName = QLocale(QLocale::Chinese).dayName(dayOfWeek); // "星期一" ~ "星期日"
    ui.label_date_show->setText(QString::number(date.month()) + "/" + QString::number(date.day()) + " " + weekName);
}

void home::updateCurrentBilibiliFans_1min()
{
    bilibili_user_follower_get_async("358071887", [this](bilibili_user_follower result) {
        ui.label_blbl_fans->setText(QString::number(result.follower_count));
    });
}

void home::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    QPixmap bg(":/images/images/6k.png");
    // // QPixmap bg(":/images/images/9565822.png");
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true); // 开启高质量缩放
    painter.drawPixmap(rect(), bg);
}

// =============================================================================
// 系统监控功能实现
// =============================================================================
#include <QValueAxis>
#include <qsplineseries.h>
#include <QCategoryAxis>
bool home::initSystemInfoUI()
{
    QHBoxLayout *layout = new QHBoxLayout(ui.widget_system_info);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    ui.widget_system_info->setLayout(layout);

    m_cpuUsageChart = new SystemChartWidget("CPU占用", "%", ui.widget_system_info);
    m_cpuUsageChart->setYAxisRange(0, 100);
    m_cpuUsageChart->setXAxisLabelInterval(240); // 每10秒显示一个x坐标
    m_cpuUsageChart->setMaxPoints(60);
    layout->addWidget(m_cpuUsageChart, 1);

    m_cpuFreqChart = new SystemChartWidget("CPU频率", "GHz", ui.widget_system_info);
    m_cpuFreqChart->setYAxisRange(1, 2); // 假设最大4GHz
    m_cpuFreqChart->setXAxisLabelInterval(240);
    m_cpuFreqChart->setMaxPoints(60);
    layout->addWidget(m_cpuFreqChart, 1);

    return true;
}

void home::startSystemMonitoring()
{
    // 启动系统实时数据流（带状态回调）
    system_realtime_start_stream_with_status(
        // 数据回调
        [this](system_realtime_info info) {
            // 使用Qt的信号槽机制确保在主线程中更新UI
            QMetaObject::invokeMethod(this, "onSystemRealtimeDataReceived", Qt::QueuedConnection, Q_ARG(system_realtime_info, info));
        },
        // 状态回调
        [this](QString status) {
            // 在主线程中更新状态指示器
            QMetaObject::invokeMethod(this, [this, status]() {
                updateConnectionStatus(status);
            },
                                      Qt::QueuedConnection);
        }
    );

    qDebug() << "系统监控已启动";
}

void home::stopSystemMonitoring()
{
    if (system_realtime_is_stream_active())
    {
        system_realtime_stop_stream();
        qDebug() << "系统监控已停止";
    }
}

void home::onSystemRealtimeDataReceived(const system_realtime_info &info)
{
    qDebug() << "onSystemRealtimeDataReceived called";

    // 检查UI组件是否已初始化
    // if (!m_cpuProgressBar || !m_memoryProgressBar || !m_cpuValueLabel || !m_memoryValueLabel) {
    //    qDebug() << "Warning: UI components not initialized, skipping data update";
    //    qDebug() << "m_cpuProgressBar:" << (void*)m_cpuProgressBar;
    //    qDebug() << "m_memoryProgressBar:" << (void*)m_memoryProgressBar;
    //    qDebug() << "m_cpuValueLabel:" << (void*)m_cpuValueLabel;
    //    qDebug() << "m_memoryValueLabel:" << (void*)m_memoryValueLabel;
    //    return;
    //}

    qDebug() << "UI components are valid, updating data...";

    // 解析CPU使用率
    QString cpuUsageStr = info.cpuUsage;
    cpuUsageStr.remove('%');
    double cpuUsage = cpuUsageStr.toDouble();

    // 解析CPU频率（如 "1.81 GHz"）
    double cpuFreq = 0.0;
    QString cpuFreqStr = info.cpuFrequency;
    if (!cpuFreqStr.isEmpty()) {
        QString freqNum = cpuFreqStr;
        freqNum.remove("GHz");
        freqNum.remove(" ");
        cpuFreq = freqNum.toDouble();
    }

    QDateTime now = QDateTime::currentDateTime();
    if (m_cpuUsageChart) {
        m_cpuUsageChart->appendDataPoint(cpuUsage, now);
    }
    if (m_cpuFreqChart) {
        m_cpuFreqChart->appendDataPoint(cpuFreq, now);
    }

    qDebug() << "系统数据已更新 - CPU:" << info.cpuUsage << "频率:" << info.cpuFrequency << "内存:" << info.memoryUsage;
}

void home::updateConnectionStatus(const QString &status)
{
    if (!m_connectionStatusLabel)
    {
        return;
    }

    QString displayText;
    QString styleSheet;

    if (status == "connected")
    {
        displayText = "已连接";
        styleSheet =
            "QLabel {"
            "    color: #27ae60;"
            "    font-size: 10px;"
            "    font-weight: bold;"
            "    padding: 2px 6px;"
            "    background-color: rgba(39, 174, 96, 30);"
            "    border-radius: 8px;"
            "    border: 1px solid rgba(39, 174, 96, 100);"
            "}";
    }
    else if (status == "reconnecting")
    {
        displayText = "重连中...";
        styleSheet =
            "QLabel {"
            "    color: #f39c12;"
            "    font-size: 10px;"
            "    font-weight: bold;"
            "    padding: 2px 6px;"
            "    background-color: rgba(243, 156, 18, 30);"
            "    border-radius: 8px;"
            "    border: 1px solid rgba(243, 156, 18, 100);"
            "}";
    }
    else if (status.startsWith("error:"))
    {
        displayText = "连接错误";
        styleSheet =
            "QLabel {"
            "    color: #e74c3c;"
            "    font-size: 10px;"
            "    font-weight: bold;"
            "    padding: 2px 6px;"
            "    background-color: rgba(231, 76, 60, 30);"
            "    border-radius: 8px;"
            "    border: 1px solid rgba(231, 76, 60, 100);"
            "}";
        // 显示详细错误信息作为工具提示
        m_connectionStatusLabel->setToolTip(status.mid(7)); // 去掉"error:"前缀
    }
    else if (status == "disconnected")
    {
        displayText = "已断开";
        styleSheet =
            "QLabel {"
            "    color: #95a5a6;"
            "    font-size: 10px;"
            "    font-weight: bold;"
            "    padding: 2px 6px;"
            "    background-color: rgba(149, 165, 166, 30);"
            "    border-radius: 8px;"
            "    border: 1px solid rgba(149, 165, 166, 100);"
            "}";
    }
    else
    {
        displayText = status;
        styleSheet = m_connectionStatusLabel->styleSheet(); // 保持当前样式
    }

    m_connectionStatusLabel->setText(displayText);
    m_connectionStatusLabel->setStyleSheet(styleSheet);

    qDebug() << "Connection status updated:" << status << "→" << displayText;
}
