#include "ui/qt/pixel_canvas.h"

#include <QImage>
#include <QPainter>
#include <QDebug>

#include "app/scene_builder.h"
#include "loaders/obj_loader.h"
#include "scene/object3d.h"

namespace gfx
{
    PixelCanvas::PixelCanvas(QQuickItem* parent)
        : QQuickPaintedItem(parent)
    {
        setAntialiasing(false);
        setOpaquePainting(true);

        RenderSettings settings;
        settings.shadingMode = ShadingMode::Phong;
        settings.renderMode = RenderMode::StaticQuality;
        settings.renderScale = 1.0f;
        settings.clearColor = colors::Black();
        settings.clearColorBuffer = true;
        settings.clearDepthBuffer = true;
        settings.depthTestEnabled = true;
        settings.wireframeEnabled = false;
        settings.backFaceCullingEnabled = true;
        settings.frontFaceIsCCW = false;

        m_idleShadingMode = settings.shadingMode;
        m_renderEngine.setRenderSettings(settings);

        // 30 FPS
        m_animationTimer.setInterval(33);
        connect(&m_animationTimer, &QTimer::timeout, this, [this]()
            {
                animateStep();
            });
    }

    void PixelCanvas::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
    {
        QQuickPaintedItem::geometryChange(newGeometry, oldGeometry);

        resizeRenderer();

        if (m_hasScene)
        {
            renderScene();
        }
        else
        {
            update();
        }
    }

    void PixelCanvas::resizeRenderer()
    {
        const RenderSettings settings = m_renderEngine.renderSettings();

        const int windowWidth = std::max(1, static_cast<int>(width()));
        const int windowHeight = std::max(1, static_cast<int>(height()));

        const int renderWidth = std::max(1, static_cast<int>(windowWidth * settings.renderScale));
        const int renderHeight = std::max(1, static_cast<int>(windowHeight * settings.renderScale));

        m_renderEngine.resize(renderWidth, renderHeight);
        m_renderEngine.setViewport({ 0, 0, renderWidth, renderHeight });
    }

    void PixelCanvas::applyRenderMode(bool interactive)
    {
        RenderSettings settings = m_renderEngine.renderSettings();

        if (interactive)
        {
            settings.renderMode = RenderMode::Interactive;
            settings.shadingMode = ShadingMode::Phong; // mantener calidad con textura
            settings.renderScale = 0.6f;
        }
        else
        {
            settings.renderMode = RenderMode::StaticQuality;
            settings.shadingMode = m_idleShadingMode;
            settings.renderScale = 1.0f;
        }

        m_renderEngine.setRenderSettings(settings);
        resizeRenderer();
    }

    void PixelCanvas::loadObj(const QString& filePath)
    {
        const Mesh mesh = ObjLoader::loadFromFile(filePath.toStdString());

        if (mesh.empty())
        {
            qWarning() << "No se pudo cargar la malla OBJ desde:" << filePath;
            return;
        }

        m_scene = SceneBuilder::buildDefaultScene(mesh);
        m_hasScene = true;

        applyRenderMode(false);
        renderScene();
        updateState();
    }

    void PixelCanvas::renderScene()
    {
        resizeRenderer();

        if (!m_hasScene)
        {
            m_renderEngine.beginFrame();
            update();
            return;
        }

        m_renderEngine.render(m_scene);
        update();
    }

    void PixelCanvas::clearScene()
    {
        m_animationTimer.stop();
        m_scene.clear();
        m_hasScene = false;

        applyRenderMode(false);
        m_renderEngine.beginFrame();
        update();
        updateState();
    }

    void PixelCanvas::setCameraPreset(int index)
    {
        if (!m_hasScene)
        {
            return;
        }

        m_scene.setActiveCamera(static_cast<size_t>(index));
        renderScene();
        updateState();
    }

    void PixelCanvas::nextCamera()
    {
        if (!m_hasScene || m_scene.cameras.empty())
        {
            return;
        }

        const size_t nextIndex = (m_scene.activeCameraIndex + 1) % m_scene.cameras.size();
        m_scene.setActiveCamera(nextIndex);
        renderScene();
        updateState();
    }

    void PixelCanvas::toggleProjection()
    {
        if (!m_hasScene)
        {
            return;
        }

        Camera* camera = m_scene.activeCamera();
        if (camera == nullptr)
        {
            return;
        }

        if (camera->projectionType == ProjectionType::Perspective)
        {
            camera->projectionType = ProjectionType::Orthographic;
            camera->left = -2.0f;
            camera->right = 2.0f;
            camera->bottom = -2.0f;
            camera->top = 2.0f;
        }
        else
        {
            camera->projectionType = ProjectionType::Perspective;
        }

        renderScene();
        updateState();
    }

    void PixelCanvas::toggleSecondaryLight()
    {
        if (!m_hasScene || m_scene.lights.size() < 2)
        {
            return;
        }

        m_scene.lights[1].enabled = !m_scene.lights[1].enabled;
        renderScene();
        updateState();
    }

    void PixelCanvas::toggleMaterial()
    {
        if (!m_hasScene || m_scene.objects.empty())
        {
            return;
        }

        Object3D& object = m_scene.objects[0];
        object.activeMaterial =
            (object.activeMaterial == MaterialType::MaterialA)
            ? MaterialType::MaterialB
            : MaterialType::MaterialA;

        renderScene();
        updateState();
    }

    void PixelCanvas::toggleAnimation()
    {
        if (!m_hasScene || m_scene.objects.empty())
        {
            return;
        }

        if (m_animationTimer.isActive())
        {
            m_animationTimer.stop();
            applyRenderMode(false);
        }
        else
        {
            applyRenderMode(true);
            m_animationTimer.start();
        }

        renderScene();
        updateState();
    }

    void PixelCanvas::animateStep()
    {
        if (!m_hasScene || m_scene.objects.empty())
        {
            return;
        }

        m_scene.objects[0].transform.rotation.y += m_rotationSpeedRadians;
        renderScene();
    }

    QString PixelCanvas::activeCameraName() const
    {
        if (!m_hasScene)
        {
            return "Sin escena";
        }

        switch (m_scene.activeCameraIndex)
        {
        case 0: return "Camara Frontal";
        case 1: return "Camara Lateral";
        case 2: return "Camara Superior";
        default: return "Camara Desconocida";
        }
    }

    QString PixelCanvas::activeMaterialName() const
    {
        if (!m_hasScene || m_scene.objects.empty())
        {
            return "Sin material";
        }

        return (m_scene.objects[0].activeMaterial == MaterialType::MaterialA)
            ? "Material A"
            : "Material B";
    }

    QString PixelCanvas::activeTextureName() const
    {
        if (!m_hasScene || m_scene.objects.empty())
        {
            return "Sin textura";
        }

        return (m_scene.objects[0].activeMaterial == MaterialType::MaterialA)
            ? "Textura A"
            : "Textura B";
    }

    QString PixelCanvas::projectionName() const
    {
        if (!m_hasScene)
        {
            return "Sin proyeccion";
        }

        const Camera* camera = m_scene.activeCamera();
        if (camera == nullptr)
        {
            return "Sin proyeccion";
        }

        return (camera->projectionType == ProjectionType::Perspective)
            ? "Perspectiva"
            : "Ortografica";
    }

    QString PixelCanvas::blueLightState() const
    {
        if (!m_hasScene || m_scene.lights.size() < 2)
        {
            return "Luz Azul: N/A";
        }

        return m_scene.lights[1].enabled
            ? "Luz Azul: ON"
            : "Luz Azul: OFF";
    }

    QString PixelCanvas::animationState() const
    {
        return m_animationTimer.isActive()
            ? "Animacion: ON"
            : "Animacion: OFF";
    }

    void PixelCanvas::updateState()
    {
        emit stateChanged();
    }

    void PixelCanvas::paint(QPainter* painter)
    {
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, false);

        const PixelBuffer& buffer = m_renderEngine.colorBuffer();

        if (buffer.empty())
        {
            painter->fillRect(QRectF(0, 0, width(), height()), Qt::black);
            return;
        }

        const QImage image(
            reinterpret_cast<const uchar*>(buffer.data()),
            buffer.width(),
            buffer.height(),
            buffer.width() * static_cast<int>(sizeof(uint32_t)),
            QImage::Format_ARGB32
        );

        painter->drawImage(
            QRect(0, 0, static_cast<int>(width()), static_cast<int>(height())),
            image
        );
    }
}