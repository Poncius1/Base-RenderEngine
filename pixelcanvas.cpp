#include "pixelcanvas.h"

#include <QPainter>
#include <QRandomGenerator>
#include <QImage>
#include <algorithm>
#include <cstdint>

namespace
{
    constexpr uint32_t kBlackARGB = 0xFF000000u;

    inline uint32_t toARGB32(const QColor& c)
    {
        // 0xAARRGGBB
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
    //Cargar la imagen original 
    QImage originalImage(imagePath);

    if (originalImage.isNull()) {
        qWarning() << "No se pudo cargar la imagen. Verifica la ruta.";
        return;
    }

    //  Asegurarnos de que nuestro buffer de memoria tenga el tamaño correcto (457x600)
    m_buffer.resize(originalImage.width(), originalImage.height());

    //  Recorrer cada píxel de la imagen original
    for (int y = 0; y < originalImage.height(); ++y) {
        for (int x = 0; x < originalImage.width(); ++x) {

            // Leer el color del píxel actual
            QColor pixelColor = originalImage.pixelColor(x, y);

            // Extraer sus componentes RGB
            int r = pixelColor.red();
            int g = pixelColor.green();
            int b = pixelColor.blue();

            if (r == 65 && g == 142 && b == 202) {
                // Lo cambiamos a otro color (0, 0, 0).
                pixelColor = QColor(0, 0, 0);
            }


            // Guardar el color modificado en  buffer de memoria
            m_buffer.setPixel(x, y, toARGB32(pixelColor));
        }
    }

    //Actualizar la pantalla
    update();
}

void PixelCanvas::paint(QPainter* painter)
{
    //ensureBuffer();

    // QImage "view" sobre la memoria del buffer (sin copiar)
    const QImage img(reinterpret_cast<const uchar*>(m_buffer.data.data()),
        m_buffer.w, m_buffer.h,
        m_buffer.w * int(sizeof(uint32_t)),
        QImage::Format_ARGB32);

    // Usamos QRect para forzar que la imagen se dibuje ajustada al tamaño del canvas
    painter->drawImage(QRect(0, 0, width(), height()), img);;
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
