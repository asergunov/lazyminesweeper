#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>

#include "Field.hpp"

int main(int argc, char *argv[])
{
    std::srand(unsigned(std::time(0)));
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterType<Field>("minesweeper", 1, 0, "Field");

    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    return app.exec();
}
