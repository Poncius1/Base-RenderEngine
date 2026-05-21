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

    const QString appDir = QCoreApplication::applicationDirPath();

    const QString projectDir = QDir::cleanPath(
        appDir + "/../.."
    );

    const QString qmlPath = QDir::cleanPath(
        projectDir + "/src/ui/qml/Main.qml"
    );

    // Tus modelos están en Base-RenderEngine/src/models
    const QString modelsDir = QDir::cleanPath(
        projectDir + "/src/models"
    );

    qDebug() << "App dir:" << appDir;
    qDebug() << "Project dir:" << projectDir;
    qDebug() << "Loading QML from:" << qmlPath;
    qDebug() << "Models dir:" << modelsDir;
    qDebug() << "Main.qml exists:" << QFile::exists(qmlPath);

    // Cambia este nombre por el OBJ real que estás usando
    qDebug() << "OBJ exists:" << QFile::exists(modelsDir + "/bunny.obj");

    engine.rootContext()->setContextProperty("modelBasePath", modelsDir);

    engine.load(QUrl::fromLocalFile(qmlPath));

    if (engine.rootObjects().isEmpty())
    {
        qDebug() << "QML did not load. Root objects are empty.";
        return -1;
    }

    return app.exec();
}