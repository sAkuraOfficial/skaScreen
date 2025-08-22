#include < QPainterPath>
#include <QPixmap>
#include <QPainter>

QPixmap getRoundPixmap(const QPixmap &src)
{
    int size = qMin(src.width(), src.height());
    QPixmap dest(size, size);
    dest.fill(Qt::transparent);

    QPainter painter(&dest);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addEllipse(0, 0, size, size);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, src.scaled(size, size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    return dest;
}


QPixmap getRoundPixmap_radius(const QPixmap &src, int radius)
{
    QSize size = src.size();
    QPixmap rounded(size);
    rounded.fill(Qt::transparent);

    QPainter painter(&rounded);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(QRectF(0, 0, size.width(), size.height()), radius, radius);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, src);
    painter.end();

    return rounded;
}