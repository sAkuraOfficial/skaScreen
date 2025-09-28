#pragma once

#include "../../core/data/data.hpp"
#include "ui_home.h"
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QProgressBar>
#include <QTimer>
#include <QPushButton>
#include <QComboBox>
#include <QMap>
#include <QVBoxLayout>
#include <QWidget>
#include <deque>
// ...existing code...
#include <QCategoryAxis>
#include <QChart>
#include <QChartView>
#include <QLineSeries>

#include "SystemChartWidget.h"
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

    // 新增：CPU占用和CPU频率图表
    SystemChartWidget* m_cpuUsageChart = nullptr;
    SystemChartWidget* m_cpuFreqChart = nullptr;
    QTimer *m_clashModeTimer = nullptr;
    bool m_isUpdatingClashCombo = false;
    bool m_isChangingClashMode = false;
    ClashProxyMode m_currentClashMode = ClashProxyMode::Rule;
    QString m_lastSelectedClashProxy;

  private slots:
    void updateCurrentWeather_30min();
    void updateCurrentTime_1s();
    void updateCurrentBilibiliFans_1min();
    void on_pushButton_clash_rule_clicked();
    void on_pushButton_clash_global_clicked();
    void on_pushButton_clash_direct_clicked();
    void refreshClashMode();
    void handleClashComboIndexChanged(int index);

  public slots:
    Q_INVOKABLE void onSystemRealtimeDataReceived(const system_realtime_info &info);

  protected:
    void paintEvent(QPaintEvent *event) override;

  private:
    bool initSystemInfoUI();
    void startSystemMonitoring();
    void stopSystemMonitoring();
    void updateConnectionStatus(const QString &status);
    void initializeClashSection();
    void startClashTrafficStream();
    void stopClashTrafficStream();
    void handleClashModeChanged(ClashProxyMode mode);
    void updateClashModeButtons(ClashProxyMode activeMode);
    void setClashMode(ClashProxyMode mode);
    void populateClashComboBox();
    QString formatBandwidth(qint64 kbps) const;
    void updateClashTrafficLabels(const clash_traffic_info &info);
    QPushButton *buttonForMode(ClashProxyMode mode) const;
};
