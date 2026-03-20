#include "ImageFilter.h"

#include <algorithm>
#include <cstdint>

namespace
{
    inline uint8_t getA(uint32_t argb) { return uint8_t((argb >> 24) & 0xFF); }
    inline uint8_t getR(uint32_t argb) { return uint8_t((argb >> 16) & 0xFF); }
    inline uint8_t getG(uint32_t argb) { return uint8_t((argb >> 8) & 0xFF); }
    inline uint8_t getB(uint32_t argb) { return uint8_t(argb & 0xFF); }

    inline uint32_t packARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
    {
        return (uint32_t(a) << 24)
            | (uint32_t(r) << 16)
            | (uint32_t(g) << 8)
            | uint32_t(b);
    }

    inline uint8_t clampToByte(float v)
    {
        if (v < 0.0f) return 0;
        if (v > 255.0f) return 255;
        return static_cast<uint8_t>(v + 0.5f);
    }
}

ConvolutionKernel ConvolutionKernel::assignmentSharpen3x3()
{
    ConvolutionKernel k;
    k.size = 3;
    k.weights = {
        -1.0f / 9.0f, -1.0f / 9.0f, -1.0f / 9.0f,
        -1.0f / 9.0f, 17.0f / 9.0f, -1.0f / 9.0f,
        -1.0f / 9.0f, -1.0f / 9.0f, -1.0f / 9.0f
    };
    return k;
}

void ImageFilter::setKernel(const ConvolutionKernel& kernel)
{
    if (kernel.size < 1 || kernel.size % 2 == 0)
        return;

    if (kernel.weights.size() != kernel.size * kernel.size)
        return;

    m_kernel = kernel;
}

void ImageFilter::apply(const PixelBuffer& src, PixelBuffer& dst) const
{
    if (src.w <= 0 || src.h <= 0)
        return;

    dst.resize(src.w, src.h);

    const int radius = m_kernel.size / 2;

    // Copiar fuente completa primero; así los bordes quedan intactos
    dst.data = src.data;

    for (int y = radius; y < src.h - radius; ++y)
    {
        for (int x = radius; x < src.w - radius; ++x)
        {
            float accR = 0.0f;
            float accG = 0.0f;
            float accB = 0.0f;

            const uint32_t centerPixel = src.data[y * src.w + x];
            const uint8_t alpha = getA(centerPixel);

            int kIndex = 0;
            for (int ky = -radius; ky <= radius; ++ky)
            {
                for (int kx = -radius; kx <= radius; ++kx, ++kIndex)
                {
                    const int sx = x + kx;
                    const int sy = y + ky;
                    const uint32_t p = src.data[sy * src.w + sx];
                    const float w = m_kernel.weights[kIndex];

                    accR += float(getR(p)) * w;
                    accG += float(getG(p)) * w;
                    accB += float(getB(p)) * w;
                }
            }

            dst.data[y * dst.w + x] = packARGB(
                alpha,
                clampToByte(accR),
                clampToByte(accG),
                clampToByte(accB)
            );
        }
    }
}