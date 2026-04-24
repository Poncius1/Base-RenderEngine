#pragma once

#include <QQuickPaintedItem>
#include <QTimer>
#include <QString>

#include "renderer/pipeline/render_engine.h"
#include "scene/scene.h"

namespace gfx
{
    class PixelCanvas : public QQuickPaintedItem
    {
        Q_OBJECT

            Q_PROPERTY(QString activeCameraName READ activeCameraName NOTIFY stateChanged)
            Q_PROPERTY(QString activeMaterialName READ activeMaterialName NOTIFY stateChanged)
            Q_PROPERTY(QString activeTextureName READ activeTextureName NOTIFY stateChanged)
            Q_PROPERTY(QString projectionName READ projectionName NOTIFY stateChanged)
            Q_PROPERTY(QString blueLightState READ blueLightState NOTIFY stateChanged)
            Q_PROPERTY(QString animationState READ animationState NOTIFY stateChanged)

    public:
        explicit PixelCanvas(QQuickItem* parent = nullptr);

        void paint(QPainter* painter) override;
        void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) override;

        Q_INVOKABLE void loadObj(const QString& filePath);
        Q_INVOKABLE void renderScene();
        Q_INVOKABLE void clearScene();

        Q_INVOKABLE void setCameraPreset(int index);
        Q_INVOKABLE void nextCamera();
        Q_INVOKABLE void toggleProjection();
        Q_INVOKABLE void toggleSecondaryLight();
        Q_INVOKABLE void toggleMaterial();
        Q_INVOKABLE void toggleAnimation();

        QString activeCameraName() const;
        QString activeMaterialName() const;
        QString activeTextureName() const;
        QString projectionName() const;
        QString blueLightState() const;
        QString animationState() const;

    signals:
        void stateChanged();

    private:
        void resizeRenderer();
        void updateState();
        void animateStep();
        void applyRenderMode(bool interactive);

    private:
        RenderEngine m_renderEngine;
        Scene m_scene;
        bool m_hasScene = false;

        QTimer m_animationTimer;
        float m_rotationSpeedRadians = 0.02f;

        ShadingMode m_idleShadingMode = ShadingMode::Phong;
    };
}