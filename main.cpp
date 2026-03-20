#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QUrl>

#include "pixelcanvas.h"

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/qt/qml/graficacion1/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    auto* window = qobject_cast<QQuickWindow*>(engine.rootObjects().first());
    if (!window) {
        return -1;
    }

    // Crear el canvas como hijo del contenido de la ventana
    auto* canvas = new PixelCanvas(window->contentItem());
    canvas->setWidth(window->width());
    canvas->setHeight(window->height());

    // Mantener el tamaño del canvas alineado a la ventana
    QObject::connect(window, &QQuickWindow::widthChanged, canvas, [window, canvas]() {
        canvas->setWidth(window->width());
        });

    QObject::connect(window, &QQuickWindow::heightChanged, canvas, [window, canvas]() {
        canvas->setHeight(window->height());
        });

    // Exponer el canvas a QML
    engine.rootContext()->setContextProperty(QStringLiteral("canvas"), canvas);

    // Demo inicial
    canvas->clear();
    canvas->loadObj("D:/Visual Projects/Graficacion1/Models/Cube_Quads.obj");

    return app.exec();
}