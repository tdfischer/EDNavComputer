#include "edjournal.h"

#include "journalentry.h"
#include "journalloader.h"
#include <QTimer>

EDJournal::EDJournal(QObject *parent) : QObject(parent)
{
    m_gameState = new GameState();
    m_location = new Location();
    m_exploration = new Exploration();
    m_loadThread.start();
}

EDJournal::~EDJournal()
{
    m_loadThread.quit();
    m_loadThread.wait();
}

GameState*
EDJournal::gameState() const
{
    return m_gameState;
}

Location*
EDJournal::location() const
{
    return m_location;
}

Exploration*
EDJournal::exploration() const
{
    return m_exploration;
}

void
EDJournal::load()
{
    JournalLoader* loader = new JournalLoader();
    loader->moveToThread(&m_loadThread);
    connect(&m_loadThread, &QThread::finished, loader, &QObject::deleteLater);
    connect(loader, &JournalLoader::finished, loader, &QObject::deleteLater);
    connect(loader, &JournalLoader::newEvent, m_gameState, &GameState::processJournalEntry);
    connect(loader, &JournalLoader::newEvent, m_location, &Location::processJournalEntry);
    connect(loader, &JournalLoader::newEvent, m_exploration, &Exploration::processJournalEntry);
    QTimer::singleShot(0, loader, &JournalLoader::load);
}
