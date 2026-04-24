#pragma once

#include <string>

#include "scene/camera.h"
#include "scene/light.h"
#include "scene/material.h"

namespace gfx
{
    class ProjectScenePresets
    {
    public:
        static Material makeMaterialA();
        static Material makeMaterialB();

        static Camera makeFrontPerspectiveCamera(float distance);
        static Camera makeSideOrthographicCamera(float extent);
        static Camera makeTopOrthographicCamera(float extent);

        static Light makeWhiteMainLight();
        static Light makeBlueSecondaryLight();

        static std::string texturePathMaterialA();
        static std::string texturePathMaterialB();
    };
}