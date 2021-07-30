#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "../libedjournal/edjournal.h"
#include "gui.h"

struct StaticInit {
    StaticInit()
    {
        Q_INIT_RESOURCE(qml);
    }
};

StaticInit staticInit;

QQmlApplicationEngine*
makeGuiEngine()
{
    QQmlApplicationEngine* engine = new QQmlApplicationEngine();
    const QUrl url(QStringLiteral("qrc:/main.qml"));

    // TODO: Create replacement UI if main.qml can't load
    /*const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);*/

    EDJournal journal;

    engine->rootContext()->setContextProperty("Journal", &journal);
    engine->load(url);
    return engine;
}
