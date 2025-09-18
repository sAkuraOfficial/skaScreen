#pragma once

#include "ui_home.h"
#include <QPainter>
#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QFrame>
#include <deque>
#include "../../core/data/data.hpp"

// 声明元类型
Q_DECLARE_METATYPE(system_realtime_info)

// 迷你图表类，用于显示CPU/内存使用率趋势
class MiniChart : public QWidget
{
    Q_OBJECT

public:
    explicit MiniChart(QWidget *parent = nullptr);
    void addDataPoint(double value);
    void setTitle(const QString &title);
    void setUnit(const QString &unit);
    void setColor(const QColor &color);
    void setMaxDataPoints(int maxPoints);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    std::deque<double> m_dataPoints;
    QString m_title;
    QString m_unit;
    QColor m_color;
    int m_maxDataPoints;
    double m_maxValue;
    double m_minValue;
};

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
    QHBoxLayout *m_systemLayout;
    QLabel *m_cpuLabel;
    QLabel *m_cpuValueLabel;
    QProgressBar *m_cpuProgressBar;
    MiniChart *m_cpuChart;
    QLabel *m_memoryLabel;
    QLabel *m_memoryValueLabel;
    QProgressBar *m_memoryProgressBar;
    MiniChart *m_memoryChart;
    QLabel *m_cpuFreqLabel;
    QLabel *m_memoryInfoLabel;
    QLabel *m_connectionStatusLabel; // 连接状态指示器

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
