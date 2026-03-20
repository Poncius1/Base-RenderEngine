#include "pixelcanvas.h"

#include <QPainter>
#include <QRandomGenerator>
#include <QImage>
#include <QDebug>

#include <algorithm>
#include <cstdint>

#include "ObjLoader.h"

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

    m_pipe.setCameraPreset(0);
    m_pipe.setCameraPresetA(0);
    m_pipe.setCameraPresetB(0);

    m_animTimer.setInterval(16);

    connect(&m_animTimer, &QTimer::timeout, this, [this]() {
        const float dt = float(m_elapsed.restart()) / 1000.0f;
        m_pipe.tick(dt);
        renderScene();
        });

    m_elapsed.start();
}

void PixelCanvas::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QQuickPaintedItem::geometryChange(newGeometry, oldGeometry);
    ensureBuffer();
    renderScene();
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

            const int r = pixelColor.red();
            const int g = pixelColor.green();
            const int b = pixelColor.blue();

            if (r == 65 && g == 142 && b == 202) {
                pixelColor = QColor(0, 0, 0);
            }

            m_buffer.setPixel(x, y, toARGB32(pixelColor));
        }
    }

    update();
}

void PixelCanvas::loadObj(const QString& path)
{
    bool ok = false;
    const Mesh mesh = ObjLoader::loadFromFile(path, &ok);

    if (!ok)
    {
        qWarning() << "Fallo al cargar OBJ:" << path;
        return;
    }

    qDebug() << "OBJ cargado correctamente. Triangulos:" << mesh.triangles.size();

    m_pipe.setMesh(mesh);
    renderScene();
}

void PixelCanvas::paint(QPainter* painter)
{
    ensureBuffer();

    painter->setRenderHint(QPainter::Antialiasing, false);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, false);

    const QImage img(
        reinterpret_cast<const uchar*>(m_buffer.data.data()),
        m_buffer.w,
        m_buffer.h,
        m_buffer.w * int(sizeof(uint32_t)),
        QImage::Format_ARGB32
    );

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

        QColor c(
            QRandomGenerator::global()->bounded(256),
            QRandomGenerator::global()->bounded(256),
            QRandomGenerator::global()->bounded(256)
        );

        if (c == Qt::black)
            c = Qt::white;

        m_buffer.setPixel(x, y, toARGB32(c));
    }

    update();
}

void PixelCanvas::toggleDualView()
{
    m_dualView = !m_dualView;
    renderScene();
}

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
        renderScene();
    }
}

void PixelCanvas::renderScene()
{
    ensureBuffer();
    m_buffer.clear(kBlackARGB);

    if (m_dualView)
        m_pipe.renderDual(m_buffer);
    else
        m_pipe.render(m_buffer);

    update();
}

void PixelCanvas::toggleProjection()
{
    m_pipe.toggleProjection();
    renderScene();
}

void PixelCanvas::setCameraPreset(int preset)
{
    if (m_dualView)
    {
        m_pipe.setCameraPresetA(preset);
        m_pipe.setCameraPresetB(preset);
    }
    else
    {
        m_pipe.setCameraPreset(preset);
    }

    renderScene();
}

void PixelCanvas::toggleShadingMode()
{
    m_pipe.toggleShadingMode();
    renderScene();
}

void PixelCanvas::increaseSpecularIntensity()
{
    m_pipe.increaseSpecularIntensity();
    renderScene();
}

void PixelCanvas::decreaseSpecularIntensity()
{
    m_pipe.decreaseSpecularIntensity();
    renderScene();
}

void PixelCanvas::toggleWhiteLight()
{
    m_pipe.toggleWhiteLight();
    renderScene();
}

void PixelCanvas::toggleRedLight()
{
    m_pipe.toggleRedLight();
    renderScene();
}