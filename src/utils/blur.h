//
// Created by Mouse Zhang on 2024/1/24.
//

#pragma once

#include <QGraphicsEffect>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>

class BlurEffect : public QGraphicsEffect
{
  public:
    ~BlurEffect()
    {
        delete_pointer(graphics_blur_effect_);
        delete_pointer(graphics_pixmap_item_);
        delete_pointer(graphics_scene_);
        delete_pointer(painter_);
        delete_pointer(res_pixmap_);
    }
    void draw(QPainter *painter) override
    {
        QPixmap pixmap;
        QPoint offset;
        double scale = painter->device()->devicePixelRatio();
        QRectF rect = sourceBoundingRect(Qt::DeviceCoordinates);
        // rect.adjust(-blur_radius, -blur_radius, blur_radius * 2, blur_radius * 2);
        rect_ = {
            rect.x() * scale - blur_radius_,
            rect.y() * scale - blur_radius_,
            rect.width() * scale + blur_radius_ * 2,
            rect.height() * scale + blur_radius_ * 2
        };
        switch (painter->device()->devType())
        {
        case QInternal::Image: {
            QImage img = static_cast<QImage *>(painter->device())->copy(rect_.toRect());
#ifdef QT_DEBUG
            img.save("capture.png");
#endif
            pixmap = blur_image(QPixmap::fromImage(img), blur_radius_);
            break;
        }
        default: {
            qWarning() << "device type is: " << painter->device()->devType() << "not implement now.";
            drawSource(painter);
            return;
        }
        }

        pixmap.setDevicePixelRatio(scale);
#ifdef QT_DEBUG
        pixmap.save("draw.png");
#endif

    // 保存 painter 当前的状态，这是一个好习惯
        painter->save();

        // 设置抗锯齿，让圆角更平滑
        painter->setRenderHint(QPainter::Antialiasing, true);

        // 创建一个圆角矩形路径
        QPainterPath path;
        // 使用 sourceBoundingRect() 获取控件的逻辑坐标矩形
        path.addRoundedRect(sourceBoundingRect(), border_radius_, border_radius_);

        // 将此路径设置为 painter 的剪切区域
        // 之后的所有绘制操作都只会在这个路径内部生效
        painter->setClipPath(path);

        // --- 新增代码结束 ---

        // 现在绘制的内容会被自动裁剪
        painter->drawPixmap(offset, pixmap);
        painter->drawPixmap(offset, sourcePixmap(Qt::LogicalCoordinates, &offset));

        // 恢复 painter 的状态，以免影响其他绘制
        painter->restore();
    }
    void set_blur_radius(const double r)
    {
        blur_radius_ = r;
    }

    void set_border_radius(const double r)
    {
        border_radius_ = r;
    }

    double get_border_radius() const
    {
        return border_radius_;
    }

  private:
    double border_radius_ = 0;
    QPixmap blur_image(const QPixmap &source_pixmap, double radius)
    {
        if (radius == 0)
        {
            return source_pixmap;
        }
        if (!first_time_ && source_pixmap.size() != res_pixmap_size_)
        {
            set_pixmap_size(source_pixmap.size());
        }
        if (first_time_)
        {
            if (!graphics_blur_effect_)
                graphics_blur_effect_ = new QGraphicsBlurEffect();
            if (!graphics_scene_)
                graphics_scene_ = new QGraphicsScene();
            if (!graphics_pixmap_item_)
                graphics_pixmap_item_ = new QGraphicsPixmapItem();
            if (!res_pixmap_)
                res_pixmap_ = new QPixmap(source_pixmap.size());
            if (!painter_)
                painter_ = new QPainter(res_pixmap_);
            graphics_pixmap_item_->setGraphicsEffect(graphics_blur_effect_);
            graphics_scene_->addItem(graphics_pixmap_item_);
            painter_->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
            painter_->setCompositionMode(QPainter::CompositionMode_Source);
            first_time_ = false;
        }
        //graphics_blur_effect_->setBlurHints(QGraphicsBlurEffect::PerformanceHint);//用这个边缘发黑
        graphics_blur_effect_->setBlurHints(QGraphicsBlurEffect::QualityHint);

        graphics_blur_effect_->setBlurRadius(radius);
        graphics_pixmap_item_->setPixmap(source_pixmap);
        graphics_scene_->render(painter_, QRectF(0, 0, source_pixmap.width(), source_pixmap.height()));
#ifdef QT_DEBUG
        if (!res_pixmap_->save("blur_capture.png"))
        {
            qWarning() << "pixmap saved error!";
        }
        res_pixmap_->copy(QRectF(blur_radius_, blur_radius_, source_pixmap.width() - blur_radius_ * 2, source_pixmap.height() - blur_radius_ * 2).toRect()).save("after_copy.png");
#endif
        QPixmap result = res_pixmap_->copy(QRectF(blur_radius_, blur_radius_, source_pixmap.width() - blur_radius_ * 2, source_pixmap.height() - blur_radius_ * 2).toRect());

        return result;
    }

    void set_pixmap_size(const QSize &size)
    {
        if (painter_)
        {
            delete painter_;
            painter_ = nullptr;
        }
        if (res_pixmap_)
        {
            delete res_pixmap_;
            res_pixmap_ = new QPixmap(size);
            painter_ = new QPainter(res_pixmap_);
            painter_->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
            painter_->setCompositionMode(QPainter::CompositionMode_Source);
        }
        else
        {
            qWarning() << "new res_pixmap_ first";
        }
    }

    template <typename T>
    void delete_pointer(T p)
    {
        static_assert(std::is_pointer<T>(), "Here should use pointer");
        if constexpr (std::is_pointer<T>())
            if (p)
            {
                delete p;
                p = nullptr;
            }
    }

    bool first_time_ = true;
    double blur_radius_ = 32;
    QGraphicsBlurEffect *graphics_blur_effect_ = nullptr;
    QGraphicsScene *graphics_scene_ = nullptr;
    QGraphicsPixmapItem *graphics_pixmap_item_ = nullptr;
    QPixmap *res_pixmap_ = nullptr;
    QPainter *painter_ = nullptr;
    QRectF rect_;
    QSize res_pixmap_size_;

    QPixmap getRoundRectPixmap(QPixmap srcPixMap, const QSize &size, int radius)
    {
        // 不处理空数据或者错误数据
        if (srcPixMap.isNull())
        {
            return srcPixMap;
        }

        // 获取图片尺寸
        int imageWidth = size.width()+10;
        int imageHeight = size.height()+10;

        // 处理大尺寸的图片,保证图片显示区域完整
        QPixmap newPixMap = srcPixMap.scaled(imageWidth, (imageHeight == 0 ? imageWidth : imageHeight), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        QPixmap destImage(imageWidth, imageHeight);
        destImage.fill(Qt::transparent);
        QPainter painter(&destImage);
        // 抗锯齿
        painter.setRenderHints(QPainter::Antialiasing, true);
        // 图片平滑处理
        painter.setRenderHints(QPainter::SmoothPixmapTransform, true);
        // 将图片裁剪为圆角
        QPainterPath path;
        QRect rect(0, 0, imageWidth, imageHeight);
        path.addRoundedRect(rect, radius, radius);
        painter.setClipPath(path);
        painter.drawPixmap(0, 0, imageWidth, imageHeight, newPixMap);
        return destImage;
    }
};
