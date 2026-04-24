#pragma once

namespace gfx
{
    // Región rectangular del framebuffer donde se dibuja la imagen.
    // Permite renderizar a pantalla completa o dividir la vista en varias áreas.
    struct Viewport
    {
        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;

        // Indica si el viewport tiene dimensiones válidas.
        bool isValid() const
        {
            return width > 0 && height > 0;
        }
    };
}