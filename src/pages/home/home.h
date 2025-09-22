#pragma once

#include "../../core/data/data.hpp"
#include "ui_home.h"
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QProgressBar>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <deque>
// ...existing code...
#include <QCategoryAxis>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
// 声明元类型
Q_DECLARE_METATYPE(system_realtime_info)
QT_FORWARD_DECLARE_CLASS(QSplineSeries)
QT_FORWARD_DECLARE_CLASS(QValueAxis)
QT_FORWARD_DECLARE_CLASS(QCategoryAxis)

class home : public QWidget
{
    Q_OBJECT

  public:
    home(QWidget *parent = nullptr);
    ~home();

  private:
    Ui::homeClass ui;
    QTimer *timer_30min; // 30分钟更新一次天气
    QTimer *timer_1s;    // 1秒更新一次时间
    QTimer *timer_1min;  // 1分钟更新一次bilibili粉丝数

    // 系统监控相关
    QLabel *m_connectionStatusLabel; // 连接状态指示器

    QChartView *m_chartView = nullptr;
    QLineSeries *m_series = nullptr;
    QCategoryAxis *m_axisX = nullptr;
    QValueAxis *m_axisY = nullptr;
    QTimer m_timer;
    QStringList m_titles;
    qreal m_step = 0.;
    qreal m_x = 5;
    qreal m_y = 1;

  private slots:
    void updateCurrentWeather_30min();
    void updateCurrentTime_1s();
    void updateCurrentBilibiliFans_1min();

  public slots:
    Q_INVOKABLE void onSystemRealtimeDataReceived(const system_realtime_info &info);

  protected:
    void paintEvent(QPaintEvent *event) override;

  private:
    bool initSystemInfoUI();
    void startSystemMonitoring();
    void stopSystemMonitoring();
    void updateConnectionStatus(const QString &status);
};
