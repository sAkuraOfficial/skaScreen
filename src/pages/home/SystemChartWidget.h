#pragma once
#include <QWidget>
#include <QChartView>
#include <QLineSeries>
#include <QCategoryAxis>
#include <QValueAxis>
#include <QVector>
#include <QDateTime>
#include <QString>
#include <QVBoxLayout>
#include <QLabel>

class SystemChartWidget : public QWidget {
    Q_OBJECT
public:
    explicit SystemChartWidget(const QString& title, const QString& unit, QWidget* parent = nullptr);
    void appendDataPoint(double value, const QDateTime& time);
    void setXAxisLabelInterval(int intervalSec); // 设置X轴标签间隔（秒）
    void setYAxisRange(double min, double max);
    void setMaxPoints(int maxPoints);

private:
    QChartView* m_chartView = nullptr;
    QLineSeries* m_series = nullptr;
    QCategoryAxis* m_axisX = nullptr;
    QValueAxis* m_axisY = nullptr;
    QVector<QDateTime> m_sampleTimes;
    int m_maxPoints = 600;
    int m_xLabelIntervalSec = 10;
    double m_x = 0;
    QLabel* m_titleLabel = nullptr;
    QString m_unit;

    void updateChart();
};
