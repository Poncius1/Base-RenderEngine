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

    // -------- Tarea 3 --------
    Q_INVOKABLE void toggleDualView();      // EXTRA A
    Q_INVOKABLE void toggleAnimation();     // EXTRA B
    Q_INVOKABLE void renderCube();
    Q_INVOKABLE void toggleProjection();
    Q_INVOKABLE void setCameraPreset(int preset);

protected:
    void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) override;

private:
    void ensureBuffer();

    PixelBuffer m_buffer;
    RenderPipeline m_pipe;

    bool m_dualView = false; // EXTRA A

    // EXTRA B
    QTimer m_animTimer;
    QElapsedTimer m_elapsed;
};