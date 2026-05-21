#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QDebug>

#include "ui/qt/pixel_canvas.h"

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<gfx::PixelCanvas>("GfxEngine", 1, 0, "PixelCanvas");

    QQmlApplicationEngine engine;

    // Carpeta donde está el .exe:
    // Base-RenderEngine/x64/Release/
    const QString appDir = QCoreApplication::applicationDirPath();

    // Ruta al Main.qml desde el .exe
    const QString qmlPath = QDir::cleanPath(
        appDir + "/../../src/ui/qml/Main.qml"
    );

    // Ruta a la carpeta de modelos desde el .exe
    const QString modelsDir = QDir::cleanPath(
        appDir + "/../../src/models"
    );

    qDebug() << "App dir:" << appDir;
    qDebug() << "Loading QML from:" << qmlPath;
    qDebug() << "Models dir:" << modelsDir;
    qDebug() << "Main.qml exists:" << QFile::exists(qmlPath);
    qDebug() << "bunny.obj exists:" << QFile::exists(modelsDir + "/bunny.obj");

    // Pasamos la carpeta de modelos a QML
    engine.rootContext()->setContextProperty("modelBasePath", modelsDir);

    engine.load(QUrl::fromLocalFile(qmlPath));

    if (engine.rootObjects().isEmpty())
    {
        qDebug() << "QML did not load. Root objects are empty.";
        return -1;
    }

    return app.exec();
}