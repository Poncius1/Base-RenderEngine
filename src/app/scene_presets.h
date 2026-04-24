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

        static Camera makeFrontRightCamera(float distance);
        static Camera makeRightProfileCamera(float distance);
        static Camera makeRearCamera(float distance);

        static Light makeWhiteMainLight();
        static Light makeBasicLight();
        static Light makeBlueSecondaryLight();

        static std::string texturePathMaterialA();
        static std::string texturePathMaterialB();
    };
}