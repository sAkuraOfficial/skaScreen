#include "SystemChartWidget.h"
#include <QChart>
#include <QDebug>
#include <QGraphicsLayout>
#include <QGraphicsSimpleTextItem>
#include <QHBoxLayout>
#include <QToolTip>

SystemChartWidget::SystemChartWidget(const QString &title, const QString &unit, QWidget *parent)
    : QWidget(parent), m_unit(unit)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
     m_titleLabel = new QLabel(title, this);
     m_titleLabel->setAlignment(Qt::AlignCenter);
     QFont titleFont = m_titleLabel->font();
     titleFont.setPointSize(6);
     m_titleLabel->setFont(titleFont);
     mainLayout->addWidget(m_titleLabel);

    m_chartView = new QChartView(this);
    m_chartView->setStyleSheet("background: transparent;");
    m_chartView->setAttribute(Qt::WA_TranslucentBackground);
    m_chartView->setFrameShape(QFrame::NoFrame);
    m_chartView->setContentsMargins(0, 0, 0, 0);
    m_chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(m_chartView, 1);

    QChart *chart = new QChart;
    chart->setTitle("");
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::NoAnimation);
    chart->setBackgroundVisible(false);
    chart->setBackgroundRoundness(0);
    chart->setMargins(QMargins(0, 0, 0, 0));
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    m_chartView->setChart(chart);
    m_chartView->setContentsMargins(0, 0, 0, 0);

    m_series = new QLineSeries;
    QPen pen(Qt::blue);
    pen.setWidth(2);
    m_series->setPen(pen);
    m_series->setUseOpenGL(false);
    m_chartView->setRenderHint(QPainter::Antialiasing, true);
    chart->addSeries(m_series);

    m_axisX = new QCategoryAxis;
    QFont xFont = m_axisX->labelsFont();
    xFont.setPointSize(6);
    m_axisX->setLabelsFont(xFont);
    chart->addAxis(m_axisX, Qt::AlignBottom);
    m_series->attachAxis(m_axisX);

    m_axisY = new QValueAxis;
    QFont yFont = m_axisY->labelsFont();
    yFont.setPointSize(6);
    m_axisY->setLabelsFont(yFont);
    m_axisY->setLabelFormat("%.1f");
    m_axisY->setTickCount(4);
    m_axisY->setLabelsAngle(0);
    chart->addAxis(m_axisY, Qt::AlignLeft);
    m_series->attachAxis(m_axisY);
    m_axisY->setRange(0, 100);

    connect(m_series, &QLineSeries::hovered, this, [this](const QPointF &point, bool state) {
        if (state)
        {
            QToolTip::showText(QCursor::pos(), QString("%1 %2").arg(point.y(), 0, 'f', 2).arg(m_unit), m_chartView);
        }
        else
        {
            QToolTip::hideText();
        }
    });
}

void SystemChartWidget::appendDataPoint(double value, const QDateTime &time)
{
    m_x += 1;
    m_series->append(m_x, value);
    m_sampleTimes.append(time);
    if (m_series->count() > m_maxPoints)
    {
        m_series->removePoints(0, m_series->count() - m_maxPoints);
        m_sampleTimes.remove(0, m_sampleTimes.size() - m_maxPoints);
    }
    updateChart();
}

void SystemChartWidget::setXAxisLabelInterval(int intervalSec)
{
    m_xLabelIntervalSec = intervalSec;
}

void SystemChartWidget::setYAxisRange(double min, double max)
{
    m_axisY->setRange(min, max);
}

void SystemChartWidget::setMaxPoints(int maxPoints)
{
    m_maxPoints = maxPoints;
}

void SystemChartWidget::updateChart()
{
    if (!m_chartView || !m_chartView->chart())
        return;
    QCategoryAxis *newAxisX = new QCategoryAxis;
    QFont xFont = newAxisX->labelsFont();
    xFont.setPointSize(6);
    newAxisX->setLabelsFont(xFont);
    int count = m_series->count();
    for (int i = 0; i < count; ++i)
    {
        int xVal = m_x - count + 1 + i;
        if (i == count - 1)
        {
            newAxisX->append(m_sampleTimes[i].toString("hh:mm:ss"), xVal);
        }
        else if (i % m_xLabelIntervalSec == 0)
        {
            newAxisX->append(m_sampleTimes[i].toString("hh:mm:ss"), xVal);
        }
    }
    newAxisX->setRange(m_x - m_maxPoints + 1, m_x);
    m_chartView->chart()->removeAxis(m_axisX);
    m_chartView->chart()->addAxis(newAxisX, Qt::AlignBottom);
    m_series->attachAxis(newAxisX);
    delete m_axisX;
    m_axisX = newAxisX;
    m_chartView->setContentsMargins(0, 0, 0, 0);
    if (m_chartView->chart())
    {
        m_chartView->chart()->setMargins(QMargins(2, 2, 2, 2));
    }

    // Y轴自适应缩放
    if (m_axisY && m_series->count() > 0)
    {
        qreal minY = m_series->at(0).y();
        qreal maxY = m_series->at(0).y();
        for (int i = 1; i < m_series->count(); ++i)
        {
            qreal y = m_series->at(i).y();
            if (y < minY)
                minY = y;
            if (y > maxY)
                maxY = y;
        }
        qreal margin = (maxY - minY) * 0.2;
        if (margin < 2)
            margin = 2;
        m_axisY->setRange(minY - margin, maxY + margin);
    }

    // 最新点标注
    if (m_chartView && m_chartView->scene() && m_series->count() > 0)
    {
        QList<QGraphicsItem *> items = m_chartView->scene()->items();
        for (auto *item : items)
        {
            auto textItem = dynamic_cast<QGraphicsSimpleTextItem *>(item);
            if (textItem && textItem->data(0).toString() == "latestValueLabel")
            {
                m_chartView->scene()->removeItem(textItem);
                delete textItem;
            }
        }
        QPointF lastPoint = m_series->points().last();
        QPointF pos = m_chartView->chart()->mapToPosition(lastPoint, m_series);
        auto *label = new QGraphicsSimpleTextItem(QString::number(lastPoint.y(), 'f', 2) + " " + m_unit);
        label->setData(0, "latestValueLabel");
        QFont font = label->font();
        font.setPointSize(10);
        font.setBold(true);
        label->setFont(font);
        label->setBrush(Qt::blue);
        label->setPos(pos.x() + 4, pos.y() - 18);
        m_chartView->scene()->addItem(label);
    }
}
