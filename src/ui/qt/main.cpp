#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>

#include "ui/qt/pixel_canvas.h"

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<gfx::PixelCanvas>("GfxEngine", 1, 0, "PixelCanvas");

    QQmlApplicationEngine engine;

    const QString qmlPath = "D:/Visual Projects/Graficacion1/src/ui/qml/Main.qml";
    qDebug() << "Loading QML from:" << qmlPath;

    engine.load(QUrl::fromLocalFile(qmlPath));

    if (engine.rootObjects().isEmpty())
    {
        qDebug() << "QML did not load. Root objects are empty.";
        return -1;
    }

    return app.exec();
}