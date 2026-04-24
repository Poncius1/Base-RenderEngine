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
        //configuracion base del canvas
        setAntialiasing(false);
        setOpaquePainting(true);

        //settings iniciales del render
        RenderSettings settings;
        settings.shadingMode = ShadingMode::Phong;
        settings.renderMode = RenderMode::StaticQuality;
        settings.renderScale = 1.0f;
        settings.clearColor = colors::Black();
        settings.clearColorBuffer = true;
        settings.clearDepthBuffer = true;
        settings.depthTestEnabled = true;
        settings.wireframeEnabled = false;
        settings.backFaceCullingEnabled = false;
        settings.frontFaceIsCCW = false;

        m_idleShadingMode = settings.shadingMode;
        m_renderEngine.setRenderSettings(settings);

        //timer para animacion
        m_animationTimer.setInterval(42);

        connect(&m_animationTimer, &QTimer::timeout, this, [this]()
            {
                animateStep();
            });
    }

    //se ejecuta cuando cambia el tamaño del widget
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

    //ajusta resolucion interna del renderer
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

    //cambia modo de render (interactivo vs calidad)
    void PixelCanvas::applyRenderMode(bool interactive)
    {
        RenderSettings settings = m_renderEngine.renderSettings();

        if (interactive)
        {
            settings.renderMode = RenderMode::Interactive;
            settings.shadingMode = ShadingMode::Gouraud;
            settings.renderScale = 0.75f;
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

    //carga modelo obj
    void PixelCanvas::loadObj(const QString& filePath)
    {
        const Mesh mesh = ObjLoader::loadFromFile(filePath.toStdString());

        if (mesh.empty())
        {
            qWarning() << "no se pudo cargar la malla obj desde:" << filePath;
            return;
        }

        //construye escena base
        m_scene = SceneBuilder::buildDefaultScene(mesh);
        m_hasScene = true;

        applyRenderMode(false);
        renderScene();
        updateState();
    }

    //renderiza la escena completa
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

    //limpia la escena actual
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

    //cambia camara activa
    void PixelCanvas::setCameraPreset(int index)
    {
        if (!m_hasScene) return;

        m_scene.setActiveCamera(static_cast<size_t>(index));
        renderScene();
        updateState();
    }

    //rota entre camaras
    void PixelCanvas::nextCamera()
    {
        if (!m_hasScene || m_scene.cameras.empty()) return;

        const size_t nextIndex = (m_scene.activeCameraIndex + 1) % m_scene.cameras.size();
        m_scene.setActiveCamera(nextIndex);
        renderScene();
        updateState();
    }

    //cambia tipo de proyeccion
    void PixelCanvas::toggleProjection()
    {
        if (!m_hasScene) return;

        Camera* camera = m_scene.activeCamera();
        if (camera == nullptr) return;

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

    //activa o desactiva luz azul
    void PixelCanvas::toggleSecondaryLight()
    {
        if (!m_hasScene || m_scene.lights.size() < 2) return;

        m_scene.lights[1].enabled = !m_scene.lights[1].enabled;
        renderScene();
        updateState();
    }

    //cambia entre materiales
    void PixelCanvas::toggleMaterial()
    {
        if (!m_hasScene || m_scene.objects.empty()) return;

        Object3D& object = m_scene.objects[0];

        object.activeMaterial =
            (object.activeMaterial == MaterialType::MaterialA)
            ? MaterialType::MaterialB
            : MaterialType::MaterialA;

        renderScene();
        updateState();
    }

    //activa o desactiva animacion
    void PixelCanvas::toggleAnimation()
    {
        if (!m_hasScene || m_scene.objects.empty()) return;

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

    //paso de animacion (rotacion)
    void PixelCanvas::animateStep()
    {
        if (!m_hasScene || m_scene.objects.empty()) return;

        m_scene.objects[0].transform.rotation.y += m_rotationSpeedRadians;
        renderScene();
    }

    //nombre de camara actual
    QString PixelCanvas::activeCameraName() const
    {
        if (!m_hasScene) return "no scene";

        switch (m_scene.activeCameraIndex)
        {
        case 0: return "Front-Right 3/4";
        case 1: return "Right Profile";
        case 2: return "Rear View";
        default: return "Unknown Camera";
        }
    }

    //nombre del material actual
    QString PixelCanvas::activeMaterialName() const
    {
        if (!m_hasScene || m_scene.objects.empty()) return "sin material";

        return (m_scene.objects[0].activeMaterial == MaterialType::MaterialA)
            ? "Material A"
            : "Material B";
    }

    //nombre de textura activa
    QString PixelCanvas::activeTextureName() const
    {
        if (!m_hasScene || m_scene.objects.empty()) return "sin textura";

        return (m_scene.objects[0].activeMaterial == MaterialType::MaterialA)
            ? "Texture A"
            : "Texture B";
    }

    //tipo de proyeccion
    QString PixelCanvas::projectionName() const
    {
        if (!m_hasScene) return "no projection";

        const Camera* camera = m_scene.activeCamera();
        if (camera == nullptr) return "no projection";

        return (camera->projectionType == ProjectionType::Perspective)
            ? "Perspective"
            : "Orthographic";
    }

    //estado de luz azul
    QString PixelCanvas::blueLightState() const
    {
        if (!m_hasScene || m_scene.lights.size() < 2)
        {
            return "Blue Light: N/A";
        }

        return m_scene.lights[1].enabled
            ? "Blue Light: On"
            : "Blue Light: Off";
    }

    //estado de animacion
    QString PixelCanvas::animationState() const
    {
        return m_animationTimer.isActive()
            ? "Animation: On"
            : "Animation: Off";
    }

    //notifica cambio de estado al ui
    void PixelCanvas::updateState()
    {
        emit stateChanged();
    }

    //dibuja el buffer en pantalla
    void PixelCanvas::paint(QPainter* painter)
    {
        const RenderSettings settings = m_renderEngine.renderSettings();

        painter->setRenderHint(QPainter::Antialiasing, false);

        //suaviza cuando hay escalado
        painter->setRenderHint(
            QPainter::SmoothPixmapTransform,
            settings.renderScale < 1.0f
        );

        const PixelBuffer& buffer = m_renderEngine.colorBuffer();

        //si no hay datos, pinta negro
        if (buffer.empty())
        {
            painter->fillRect(QRectF(0, 0, width(), height()), Qt::black);
            return;
        }

        //convierte buffer a imagen qt
        const QImage image(
            reinterpret_cast<const uchar*>(buffer.data()),
            buffer.width(),
            buffer.height(),
            buffer.width() * static_cast<int>(sizeof(uint32_t)),
            QImage::Format_ARGB32
        );

        //dibuja en pantalla
        painter->drawImage(
            QRect(0, 0, static_cast<int>(width()), static_cast<int>(height())),
            image
        );
    }
}