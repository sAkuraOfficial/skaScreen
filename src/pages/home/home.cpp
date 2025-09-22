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
    QLayout *layout = new QHBoxLayout(ui.widget_system_info);
    //去除所有边距
    layout->setContentsMargins(0, 0, 0, 0);
    // 去除所有间距
    layout->setSpacing(0);
    ui.widget_system_info->setLayout(layout);
    


    auto chart = new QChart;
    chart->setTitle("");
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::NoAnimation);
    chart->setBackgroundVisible(false); // 不绘制chart背景，透明
    // 设置QChartView透明，支持父widget圆角和背景色
    m_chartView = new QChartView(ui.widget_system_info);
    m_chartView->setChart(chart);
    m_chartView->setStyleSheet("background: transparent;");
    m_chartView->setAttribute(Qt::WA_TranslucentBackground);
    m_chartView->setFrameShape(QFrame::NoFrame);
    chart->setBackgroundRoundness(8);

    m_axisX = new QCategoryAxis;
    m_axisY = new QValueAxis;
    // 设置X轴字体更小
    QFont xFont = m_axisX->labelsFont();
    xFont.setPointSize(3);
    m_axisX->setLabelsFont(xFont);
    // 设置Y轴字体更小，且显示数值
    QFont yFont = m_axisY->labelsFont();
    yFont.setPointSize(6);
    m_axisY->setLabelsFont(yFont);
    m_axisY->setLabelFormat("%.1f"); // 前后加空格，防止省略号
    m_axisY->setTickCount(4); // 让Y轴有更多刻度
    m_axisY->setLabelsAngle(0); // 水平显示

    m_series = new QLineSeries;
    QPen pen(Qt::red);
    pen.setWidth(2);
    m_series->setPen(pen);
    // 抗锯齿设置
    m_series->setUseOpenGL(false); // 关闭OpenGL，避免锯齿
    m_chartView->setRenderHint(QPainter::Antialiasing, true);
    // 鼠标悬浮显示数值
    connect(m_series, &QLineSeries::hovered, this, [this](const QPointF &point, bool state) {
        if (state) {
            QToolTip::showText(QCursor::pos(), QString("%1").arg(point.y(), 0, 'f', 2), m_chartView);
        } else {
            QToolTip::hideText();
        }
    });

    chart->addSeries(m_series);
    chart->addAxis(m_axisX, Qt::AlignBottom);
    chart->addAxis(m_axisY, Qt::AlignLeft);
    m_series->attachAxis(m_axisX);
    m_series->attachAxis(m_axisY);
    m_axisY->setRange(0, 100);

    layout->addWidget(m_chartView);
    chart->setMargins(QMargins(0, 0, 1, 0)); // 左边距加大，避免y轴被裁剪
    chart->layout()->setContentsMargins(0, 0, 1, 0);
    m_chartView->setContentsMargins(0, 0, 1, 0);
    chart->legend()->hide();
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


    // 推窗动画：当前数据永远在最右边，旧数据左移
    const int maxPoints = 600; // 显示60个点
    QDateTime now = QDateTime::currentDateTime();
    QString timeLabel = now.toString("hh:mm");
    m_x += 1;
    m_y = cpuUsage;
    // 采样时间队列
    static QVector<QDateTime> m_sampleTimes;
    m_sampleTimes.append(now);
    if (m_sampleTimes.size() > maxPoints) {
        m_sampleTimes.remove(0, m_sampleTimes.size() - maxPoints);
    }
    if (m_series) {
        m_series->append(m_x, m_y);
        if (m_series->count() > maxPoints) {
            m_series->removePoints(0, m_series->count() - maxPoints);
        }

        // 在最新点上标注数值
        if (m_chartView && m_chartView->scene() && m_series->count() > 0) {
            // 先移除旧的标注
            QList<QGraphicsItem*> items = m_chartView->scene()->items();
            for (auto *item : items) {
                auto textItem = dynamic_cast<QGraphicsSimpleTextItem*>(item);
                if (textItem && textItem->data(0).toString() == "latestValueLabel") {
                    m_chartView->scene()->removeItem(textItem);
                    delete textItem;
                }
            }
            // 添加新标注
            QPointF lastPoint = m_series->points().last();
            QPointF pos = m_chartView->chart()->mapToPosition(lastPoint, m_series);
            auto *label = new QGraphicsSimpleTextItem(QString::number(lastPoint.y(), 'f', 2));
            label->setData(0, "latestValueLabel");
            QFont font = label->font();
            font.setPointSize(10);
            font.setBold(true);
            label->setFont(font);
            label->setBrush(Qt::red);
            label->setPos(pos.x() + 4, pos.y() - 18);
            m_chartView->scene()->addItem(label);
        }
        // X轴显示时间字符串（每10秒显示一个x坐标，且标签内容不会随动画移动而变化）
        if (m_chartView && m_chartView->chart()) {
            QCategoryAxis* newAxisX = new QCategoryAxis;
            QFont xFont = newAxisX->labelsFont();
            xFont.setPointSize(6);
            newAxisX->setLabelsFont(xFont);
            int startIdx = qMax(0, m_series->count() - maxPoints);
            for (int i = 0; i < m_series->count(); ++i) {
                int xVal = m_x - m_series->count() + 1 + i;
               if (i % 50 == 0) {
                    // 每10秒显示一个x坐标
                    newAxisX->append(m_sampleTimes[i].toString("hh:mm:ss"), xVal);
                }
            }
            newAxisX->setRange(m_x - maxPoints + 1, m_x);
            // 替换旧X轴
            m_chartView->chart()->removeAxis(m_axisX);
            m_chartView->chart()->addAxis(newAxisX, Qt::AlignBottom);
            m_series->attachAxis(newAxisX);
            delete m_axisX;
            m_axisX = newAxisX;
        }

        // Y轴自适应缩放
        if (m_axisY && m_series->count() > 0) {
            qreal minY = m_series->at(0).y();
            qreal maxY = m_series->at(0).y();
            for (int i = 1; i < m_series->count(); ++i) {
                qreal y = m_series->at(i).y();
                if (y < minY) minY = y;
                if (y > maxY) maxY = y;
            }
            qreal margin = (maxY - minY) * 0.2;
            if (margin < 2) margin = 2;
            m_axisY->setRange(minY - margin, maxY + margin);
        }
    }

    qDebug() << "系统数据已更新 - CPU:" << info.cpuUsage << "内存:" << info.memoryUsage;
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
