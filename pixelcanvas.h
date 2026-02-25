#pragma once
#include <QQuickPaintedItem>
#include <QColor>

#include "pixelbuffer.h"

class PixelCanvas final : public QQuickPaintedItem
{
    Q_OBJECT

public:
    explicit PixelCanvas(QQuickItem* parent = nullptr);

    void paint(QPainter* painter) override;

    Q_INVOKABLE void clear();
    Q_INVOKABLE void setPixel(int x, int y, const QColor& color);
    Q_INVOKABLE void randomPixels(int count);

private:
    void ensureBuffer();

    PixelBuffer m_buffer;
};
