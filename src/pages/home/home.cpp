#include "home.h"
#include "../../core/data/data.hpp"
#include "../../core/data/data_fetch/data_fetch.hpp"
#include "../../utils/blur.h"
#include "../../utils/img.h"
#include <QLocale>
#include <qstyleoption.h>
#include <QTimer>
#include <tyme.h>
home::home(QWidget *parent)
    : QWidget(parent)
    , m_systemLayout(nullptr)
    , m_cpuLabel(nullptr)
    , m_cpuValueLabel(nullptr)
    , m_cpuProgressBar(nullptr)
    , m_cpuChart(nullptr)
    , m_memoryLabel(nullptr)
    , m_memoryValueLabel(nullptr)
    , m_memoryProgressBar(nullptr)
    , m_memoryChart(nullptr)
    , m_cpuFreqLabel(nullptr)
    , m_memoryInfoLabel(nullptr)
    , m_connectionStatusLabel(nullptr)
{
    ui.setupUi(this);
    
    // 注册自定义类型以便在信号槽中使用
    qRegisterMetaType<system_realtime_info>("system_realtime_info");

    // 覆盖一下ui文件设置的样式表。
    setStyleSheet(QString::fromUtf8("#widget,#widget_2,#widget_3,#widget_4,#widget_5,#widget_system_info{\n"
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

    //更新粉丝数量
    timer_1min = new QTimer(this);
    connect(timer_1min, &QTimer::timeout, this, &home::updateCurrentBilibiliFans_1min);
    timer_1min->start(1000 * 60);//一分钟
    updateCurrentBilibiliFans_1min();//启动的时候立刻更新一次
    
    // 初始化系统监控UI和数据流
    qDebug() << "开始初始化系统监控UI...";
    if (initSystemInfoUI()) {
        qDebug() << "系统监控UI初始化成功，启动监控...";
        startSystemMonitoring();
    } else {
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
    if (lunar_day_str.substr(0, lunar_prefix.length()) == lunar_prefix) {
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
// MiniChart 迷你图表实现
// =============================================================================

MiniChart::MiniChart(QWidget *parent)
    : QWidget(parent)
    , m_color(QColor(0, 120, 215))
    , m_maxDataPoints(30)
    , m_maxValue(0.0)
    , m_minValue(0.0)
{
    setFixedSize(80, 40);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
}

void MiniChart::addDataPoint(double value)
{
    m_dataPoints.push_back(value);
    
    if (m_dataPoints.size() > m_maxDataPoints) {
        m_dataPoints.pop_front();
    }
    
    // 更新最大最小值
    if (m_dataPoints.size() == 1) {
        m_maxValue = m_minValue = value;
    } else {
        m_maxValue = std::max(m_maxValue, value);
        m_minValue = std::min(m_minValue, value);
    }
    
    update();
}

void MiniChart::setTitle(const QString &title)
{
    m_title = title;
}

void MiniChart::setUnit(const QString &unit)
{
    m_unit = unit;
}

void MiniChart::setColor(const QColor &color)
{
    m_color = color;
}

void MiniChart::setMaxDataPoints(int maxPoints)
{
    m_maxDataPoints = maxPoints;
}

void MiniChart::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    if (m_dataPoints.size() < 2) {
        return;
    }
    
    QRect chartRect = rect().adjusted(2, 2, -2, -2);
    
    // 绘制背景
    painter.fillRect(chartRect, QColor(255, 255, 255, 50));
    
    // 计算缩放比例
    double valueRange = m_maxValue - m_minValue;
    if (valueRange < 0.1) valueRange = 0.1; // 避免除零
    
    double xStep = (double)chartRect.width() / (m_maxDataPoints - 1);
    
    // 绘制折线
    QPen pen(m_color, 2);
    painter.setPen(pen);
    
    QPolygonF line;
    for (size_t i = 0; i < m_dataPoints.size(); ++i) {
        double x = chartRect.left() + i * xStep;
        double y = chartRect.bottom() - ((m_dataPoints[i] - m_minValue) / valueRange) * chartRect.height();
        line << QPointF(x, y);
    }
    
    if (line.size() > 1) {
        painter.drawPolyline(line);
        
        // 填充渐变
        QLinearGradient gradient(0, chartRect.top(), 0, chartRect.bottom());
        gradient.setColorAt(0, QColor(m_color.red(), m_color.green(), m_color.blue(), 100));
        gradient.setColorAt(1, QColor(m_color.red(), m_color.green(), m_color.blue(), 20));
        
        QPolygonF fillArea = line;
        fillArea << QPointF(chartRect.right(), chartRect.bottom());
        fillArea << QPointF(chartRect.left(), chartRect.bottom());
        
        QPainterPath path;
        path.addPolygon(fillArea);
        painter.fillPath(path, gradient);
    }
}

// =============================================================================
// 系统监控功能实现
// =============================================================================

bool home::initSystemInfoUI()
{
    qDebug() << "=== initSystemInfoUI 开始 ===";
    
    // 查找widget_system_info
    QWidget *systemInfoWidget = findChild<QWidget*>("widget_system_info");
    if (!systemInfoWidget) {
        qDebug() << "Error: widget_system_info not found in UI file";
        qDebug() << "Available widgets:";
        QList<QWidget*> widgets = findChildren<QWidget*>();
        for (QWidget* widget : widgets) {
            if (!widget->objectName().isEmpty()) {
                qDebug() << "  -" << widget->objectName();
            }
        }
        return false;
    }
    
    qDebug() << "Found widget_system_info, size:" << systemInfoWidget->size();
    qDebug() << "widget_system_info visible:" << systemInfoWidget->isVisible();
    qDebug() << "widget_system_info geometry:" << systemInfoWidget->geometry();
    
    // 创建主布局
    m_systemLayout = new QHBoxLayout();
    systemInfoWidget->setLayout(m_systemLayout);
    m_systemLayout->setContentsMargins(8, 4, 8, 4);
    m_systemLayout->setSpacing(12);
    
    // CPU 监控区域
    QWidget *cpuWidget = new QWidget();
    cpuWidget->setFixedWidth(160);
    QVBoxLayout *cpuLayout = new QVBoxLayout(cpuWidget);
    cpuLayout->setContentsMargins(0, 0, 0, 0);
    cpuLayout->setSpacing(2);
    
    // CPU 标题和数值
    QWidget *cpuTopWidget = new QWidget();
    QHBoxLayout *cpuTopLayout = new QHBoxLayout(cpuTopWidget);
    cpuTopLayout->setContentsMargins(0, 0, 0, 0);
    cpuTopLayout->setSpacing(4);
    
    m_cpuLabel = new QLabel("CPU");
    m_cpuLabel->setStyleSheet("font-weight: bold; color: #333; font-size: 12px;");
    m_cpuValueLabel = new QLabel("0%");
    m_cpuValueLabel->setStyleSheet("color: #0078d4; font-weight: bold; font-size: 12px;");
    m_cpuValueLabel->setAlignment(Qt::AlignRight);
    
    cpuTopLayout->addWidget(m_cpuLabel);
    cpuTopLayout->addStretch();
    cpuTopLayout->addWidget(m_cpuValueLabel);
    
    // CPU 进度条
    m_cpuProgressBar = new QProgressBar();
    m_cpuProgressBar->setFixedHeight(6);
    m_cpuProgressBar->setRange(0, 100);
    m_cpuProgressBar->setTextVisible(false);
    m_cpuProgressBar->setStyleSheet(
        "QProgressBar {"
        "    border: none;"
        "    background-color: #e0e0e0;"
        "    border-radius: 3px;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: #0078d4;"
        "    border-radius: 3px;"
        "}"
    );
    
    // CPU 频率标签
    m_cpuFreqLabel = new QLabel("0 GHz");
    m_cpuFreqLabel->setStyleSheet("color: #666; font-size: 10px;");
    m_cpuFreqLabel->setAlignment(Qt::AlignCenter);
    
    cpuLayout->addWidget(cpuTopWidget);
    cpuLayout->addWidget(m_cpuProgressBar);
    cpuLayout->addWidget(m_cpuFreqLabel);
    
    // CPU 图表
    m_cpuChart = new MiniChart();
    m_cpuChart->setColor(QColor(0, 120, 215));
    m_cpuChart->setMaxDataPoints(25);
    
    // 内存监控区域
    QWidget *memoryWidget = new QWidget();
    memoryWidget->setFixedWidth(160);
    QVBoxLayout *memoryLayout = new QVBoxLayout(memoryWidget);
    memoryLayout->setContentsMargins(0, 0, 0, 0);
    memoryLayout->setSpacing(2);
    
    // 内存标题和数值
    QWidget *memoryTopWidget = new QWidget();
    QHBoxLayout *memoryTopLayout = new QHBoxLayout(memoryTopWidget);
    memoryTopLayout->setContentsMargins(0, 0, 0, 0);
    memoryTopLayout->setSpacing(4);
    
    m_memoryLabel = new QLabel("内存");
    m_memoryLabel->setStyleSheet("font-weight: bold; color: #333; font-size: 12px;");
    m_memoryValueLabel = new QLabel("0%");
    m_memoryValueLabel->setStyleSheet("color: #e74c3c; font-weight: bold; font-size: 12px;");
    m_memoryValueLabel->setAlignment(Qt::AlignRight);
    
    memoryTopLayout->addWidget(m_memoryLabel);
    memoryTopLayout->addStretch();
    memoryTopLayout->addWidget(m_memoryValueLabel);
    
    // 内存进度条
    m_memoryProgressBar = new QProgressBar();
    m_memoryProgressBar->setFixedHeight(6);
    m_memoryProgressBar->setRange(0, 100);
    m_memoryProgressBar->setTextVisible(false);
    m_memoryProgressBar->setStyleSheet(
        "QProgressBar {"
        "    border: none;"
        "    background-color: #e0e0e0;"
        "    border-radius: 3px;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: #e74c3c;"
        "    border-radius: 3px;"
        "}"
    );
    
    // 内存信息标签
    m_memoryInfoLabel = new QLabel("0 GB / 0 GB");
    m_memoryInfoLabel->setStyleSheet("color: #666; font-size: 10px;");
    m_memoryInfoLabel->setAlignment(Qt::AlignCenter);
    
    memoryLayout->addWidget(memoryTopWidget);
    memoryLayout->addWidget(m_memoryProgressBar);
    memoryLayout->addWidget(m_memoryInfoLabel);
    
    // 内存图表
    m_memoryChart = new MiniChart();
    m_memoryChart->setColor(QColor(231, 76, 60));
    m_memoryChart->setMaxDataPoints(25);
    
    // 连接状态指示器
    m_connectionStatusLabel = new QLabel("连接中...");
    m_connectionStatusLabel->setStyleSheet(
        "QLabel {"
        "    color: #f39c12;"
        "    font-size: 10px;"
        "    font-weight: bold;"
        "    padding: 2px 6px;"
        "    background-color: rgba(243, 156, 18, 30);"
        "    border-radius: 8px;"
        "    border: 1px solid rgba(243, 156, 18, 100);"
        "}"
    );
    m_connectionStatusLabel->setAlignment(Qt::AlignCenter);
    m_connectionStatusLabel->setFixedHeight(20);
    
    // 添加到主布局
    m_systemLayout->addWidget(cpuWidget);
    m_systemLayout->addWidget(m_cpuChart);
    m_systemLayout->addWidget(memoryWidget);
    m_systemLayout->addWidget(m_memoryChart);
    m_systemLayout->addWidget(m_connectionStatusLabel);
    m_systemLayout->addStretch();
    
    // 设置widget样式
    systemInfoWidget->setStyleSheet(
        "QWidget {"
        "    background-color: rgba(255, 255, 255, 200);"
        "    border-radius: 8px;"
        "    border: 1px solid rgba(200, 200, 200, 100);"
        "}"
    );
    
    qDebug() << "System monitoring UI initialized successfully";
    qDebug() << "Created UI components:";
    qDebug() << "  m_cpuProgressBar:" << (void*)m_cpuProgressBar;
    qDebug() << "  m_memoryProgressBar:" << (void*)m_memoryProgressBar;
    qDebug() << "  m_cpuValueLabel:" << (void*)m_cpuValueLabel;
    qDebug() << "  m_memoryValueLabel:" << (void*)m_memoryValueLabel;
    qDebug() << "=== initSystemInfoUI 完成 ===";
    return true;
}

void home::startSystemMonitoring()
{
    // 启动系统实时数据流（带状态回调）
    system_realtime_start_stream_with_status(
        // 数据回调
        [this](system_realtime_info info) {
            // 使用Qt的信号槽机制确保在主线程中更新UI
            QMetaObject::invokeMethod(this, "onSystemRealtimeDataReceived", 
                                      Qt::QueuedConnection,
                                      Q_ARG(system_realtime_info, info));
        },
        // 状态回调
        [this](QString status) {
            // 在主线程中更新状态指示器
            QMetaObject::invokeMethod(this, [this, status]() {
                updateConnectionStatus(status);
            }, Qt::QueuedConnection);
        }
    );
    
    qDebug() << "系统监控已启动";
}

void home::stopSystemMonitoring()
{
    if (system_realtime_is_stream_active()) {
        system_realtime_stop_stream();
        qDebug() << "系统监控已停止";
    }
}

void home::onSystemRealtimeDataReceived(const system_realtime_info &info)
{
    qDebug() << "onSystemRealtimeDataReceived called";
    
    // 检查UI组件是否已初始化
    if (!m_cpuProgressBar || !m_memoryProgressBar || !m_cpuValueLabel || !m_memoryValueLabel) {
        qDebug() << "Warning: UI components not initialized, skipping data update";
        qDebug() << "m_cpuProgressBar:" << (void*)m_cpuProgressBar;
        qDebug() << "m_memoryProgressBar:" << (void*)m_memoryProgressBar;
        qDebug() << "m_cpuValueLabel:" << (void*)m_cpuValueLabel;
        qDebug() << "m_memoryValueLabel:" << (void*)m_memoryValueLabel;
        return;
    }
    
    qDebug() << "UI components are valid, updating data...";
    
    // 解析CPU使用率
    QString cpuUsageStr = info.cpuUsage;
    cpuUsageStr.remove('%');
    double cpuUsage = cpuUsageStr.toDouble();
    
    // 解析内存使用率
    QString memoryUsageStr = info.memoryUsage;
    memoryUsageStr.remove('%');
    double memoryUsage = memoryUsageStr.toDouble();
    
    // 更新CPU信息
    m_cpuValueLabel->setText(info.cpuUsage);
    m_cpuProgressBar->setValue(static_cast<int>(cpuUsage));
    m_cpuFreqLabel->setText(info.cpuFrequency);
    if (m_cpuChart) {
        m_cpuChart->addDataPoint(cpuUsage);
    }
    
    // 更新内存信息
    m_memoryValueLabel->setText(info.memoryUsage);
    m_memoryProgressBar->setValue(static_cast<int>(memoryUsage));
    m_memoryInfoLabel->setText(info.usedMemory + " / " + info.totalMemory);
    if (m_memoryChart) {
        m_memoryChart->addDataPoint(memoryUsage);
    }
    
    qDebug() << "系统数据已更新 - CPU:" << info.cpuUsage << "内存:" << info.memoryUsage;
}

void home::updateConnectionStatus(const QString &status)
{
    if (!m_connectionStatusLabel) {
        return;
    }
    
    QString displayText;
    QString styleSheet;
    
    if (status == "connected") {
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
    } else if (status == "reconnecting") {
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
    } else if (status.startsWith("error:")) {
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
    } else if (status == "disconnected") {
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
    } else {
        displayText = status;
        styleSheet = m_connectionStatusLabel->styleSheet(); // 保持当前样式
    }
    
    m_connectionStatusLabel->setText(displayText);
    m_connectionStatusLabel->setStyleSheet(styleSheet);
    
    qDebug() << "Connection status updated:" << status << "→" << displayText;
}

