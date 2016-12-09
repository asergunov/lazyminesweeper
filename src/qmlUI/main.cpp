#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "Field.hpp"

int main(int argc, char *argv[])
{
    std::srand(unsigned(std::time(0)));
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    Field field;

    engine.rootContext()->setContextProperty("Field", &field);
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    return app.exec();
}
