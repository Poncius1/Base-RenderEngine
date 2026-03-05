#include "pixelcanvas.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QImage>
#include <QDebug>
#include <algorithm>
#include <cstdint>

namespace
{
    constexpr uint32_t kBlackARGB = 0xFF000000u;

    inline uint32_t toARGB32(const QColor& c)
    {
        return (uint32_t(c.alpha()) << 24)
            | (uint32_t(c.red()) << 16)
            | (uint32_t(c.green()) << 8)
            | (uint32_t(c.blue()));
    }
}

PixelCanvas::PixelCanvas(QQuickItem* parent)
    : QQuickPaintedItem(parent)
{
    setAntialiasing(false);
    setRenderTarget(QQuickPaintedItem::FramebufferObject);

    // Base
    m_pipe.setCameraPreset(0);

    // EXTRA A: presets iniciales para dual
    m_pipe.setCameraPresetA(0);
    m_pipe.setCameraPresetB(0);

    // EXTRA B: timer (~60fps)
    m_animTimer.setInterval(16);

    connect(&m_animTimer, &QTimer::timeout, this, [this]() {
        // dt en segundos
        float dt = float(m_elapsed.restart()) / 1000.0f;
        m_pipe.tick(dt);
        renderCube(); // re-render continuo (respeta dual view)
        });

    m_elapsed.start();
}

void PixelCanvas::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QQuickPaintedItem::geometryChange(newGeometry, oldGeometry);
    ensureBuffer();
    renderCube();
}

void PixelCanvas::ensureBuffer()
{
    const int w = std::max(1, int(width()));
    const int h = std::max(1, int(height()));

    if (m_buffer.w == w && m_buffer.h == h)
        return;

    m_buffer.resize(w, h);
    m_buffer.clear(kBlackARGB);
}

void PixelCanvas::processImage(const QString& imagePath)
{
    ensureBuffer();

    QImage originalImage(imagePath);
    if (originalImage.isNull()) {
        qWarning() << "No se pudo cargar la imagen. Verifica la ruta.";
        return;
    }

    const int copyW = std::min(m_buffer.w, originalImage.width());
    const int copyH = std::min(m_buffer.h, originalImage.height());

    for (int y = 0; y < copyH; ++y) {
        for (int x = 0; x < copyW; ++x) {

            QColor pixelColor = originalImage.pixelColor(x, y);

            int r = pixelColor.red();
            int g = pixelColor.green();
            int b = pixelColor.blue();

            if (r == 65 && g == 142 && b == 202) {
                pixelColor = QColor(0, 0, 0);
            }

            m_buffer.setPixel(x, y, toARGB32(pixelColor));
        }
    }

    update();
}

void PixelCanvas::paint(QPainter* painter)
{
    ensureBuffer();

    painter->setRenderHint(QPainter::Antialiasing, false);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, false);

    const QImage img(reinterpret_cast<const uchar*>(m_buffer.data.data()),
        m_buffer.w, m_buffer.h,
        m_buffer.w * int(sizeof(uint32_t)),
        QImage::Format_ARGB32);

    painter->drawImage(QRect(0, 0, int(width()), int(height())), img);
}

void PixelCanvas::clear()
{
    ensureBuffer();
    m_buffer.clear(kBlackARGB);
    update();
}

void PixelCanvas::setPixel(int x, int y, const QColor& color)
{
    ensureBuffer();
    m_buffer.setPixel(x, y, toARGB32(color));
    update();
}

void PixelCanvas::randomPixels(int count)
{
    ensureBuffer();
    count = std::max(0, count);

    for (int i = 0; i < count; ++i) {
        const int x = QRandomGenerator::global()->bounded(m_buffer.w);
        const int y = QRandomGenerator::global()->bounded(m_buffer.h);

        QColor c(QRandomGenerator::global()->bounded(256),
            QRandomGenerator::global()->bounded(256),
            QRandomGenerator::global()->bounded(256));

        if (c == Qt::black) c = Qt::white;
        m_buffer.setPixel(x, y, toARGB32(c));
    }

    update();
}

// EXTRA A toggle
void PixelCanvas::toggleDualView()
{
    m_dualView = !m_dualView;
    renderCube();
}

// EXTRA B toggle
void PixelCanvas::toggleAnimation()
{
    m_pipe.toggleAnimation();

    if (m_pipe.isAnimating())
    {
        m_elapsed.restart();
        m_animTimer.start();
    }
    else
    {
        m_animTimer.stop();
        renderCube(); // render final estático
    }
}

void PixelCanvas::renderCube()
{
    ensureBuffer();
    m_buffer.clear(kBlackARGB);

    if (m_dualView)
        m_pipe.renderCubePointsDual(m_buffer);
    else
        m_pipe.renderCubePoints(m_buffer);

    update();
}

void PixelCanvas::toggleProjection()
{
    m_pipe.toggleProjection();
    renderCube();
}

void PixelCanvas::setCameraPreset(int preset)
{
    // En dual, actualiza A y B con el mismo preset (simple)
    if (m_dualView)
    {
        m_pipe.setCameraPresetA(preset);
        m_pipe.setCameraPresetB(preset);
    }
    else
    {
        m_pipe.setCameraPreset(preset);
    }

    renderCube();
}