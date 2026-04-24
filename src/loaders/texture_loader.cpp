#include "loaders/texture_loader.h"

#include <QImage>
#include <QDebug>

#include "core/image/color.h"
#include "core/image/pixel_buffer.h"

namespace gfx
{
    Texture TextureLoader::loadFromFile(const std::string& filePath)
    {
        Texture texture;

        qDebug() << "Loading texture from:" << QString::fromStdString(filePath);

        QImage image(QString::fromStdString(filePath));
        if (image.isNull())
        {
            qDebug() << "Failed to load texture:" << QString::fromStdString(filePath);
            return texture;
        }

        qDebug() << "Texture loaded successfully. Size:" << image.width() << "x" << image.height();

        // Convertimos a formato consistente para el framebuffer interno.
        const QImage converted = image.convertToFormat(QImage::Format_ARGB32);

        PixelBuffer buffer(converted.width(), converted.height());

        for (int y = 0; y < converted.height(); ++y)
        {
            for (int x = 0; x < converted.width(); ++x)
            {
                const QRgb pixel = converted.pixel(x, y);

                const Color color = colorFromBytes(
                    static_cast<uint8_t>(qRed(pixel)),
                    static_cast<uint8_t>(qGreen(pixel)),
                    static_cast<uint8_t>(qBlue(pixel)),
                    static_cast<uint8_t>(qAlpha(pixel))
                );

                buffer.setPixel(x, y, color);
            }
        }

        texture.setBuffer(buffer);
        return texture;
    }
}