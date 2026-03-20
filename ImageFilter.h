#pragma once

#include "pixelbuffer.h"
#include <QVector>

struct ConvolutionKernel
{
    int size = 3;              // debe ser impar: 3, 5, 7...
    QVector<float> weights;    // size * size

    static ConvolutionKernel assignmentSharpen3x3();
};

class ImageFilter
{
public:
    void setKernel(const ConvolutionKernel& kernel);
    const ConvolutionKernel& kernel() const { return m_kernel; }

    void apply(const PixelBuffer& src, PixelBuffer& dst) const;

private:
    ConvolutionKernel m_kernel = ConvolutionKernel::assignmentSharpen3x3();
};