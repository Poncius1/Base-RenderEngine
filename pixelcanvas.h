#pragma once

#include <QQuickPaintedItem>
#include <QColor>
#include <QTimer>
#include <QElapsedTimer>

#include "pixelbuffer.h"
#include "RenderPipeline.h"

class PixelCanvas final : public QQuickPaintedItem
{
    Q_OBJECT

public:
    explicit PixelCanvas(QQuickItem* parent = nullptr);

    void paint(QPainter* painter) override;

    Q_INVOKABLE void clear();
    Q_INVOKABLE void setPixel(int x, int y, const QColor& color);
    Q_INVOKABLE void randomPixels(int count);
    Q_INVOKABLE void processImage(const QString& imagePath);

    Q_INVOKABLE void loadObj(const QString& path);

    Q_INVOKABLE void toggleDualView();
    Q_INVOKABLE void toggleAnimation();
    Q_INVOKABLE void renderScene();
    Q_INVOKABLE void toggleProjection();
    Q_INVOKABLE void setCameraPreset(int preset);

    Q_INVOKABLE void toggleShadingMode();
    Q_INVOKABLE void increaseSpecularIntensity();
    Q_INVOKABLE void decreaseSpecularIntensity();
    Q_INVOKABLE void toggleWhiteLight();
    Q_INVOKABLE void toggleRedLight();

protected:
    void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) override;

private:
    void ensureBuffer();

private:
    PixelBuffer m_buffer;
    RenderPipeline m_pipe;

    bool m_dualView = false;

    QTimer m_animTimer;
    QElapsedTimer m_elapsed;
};