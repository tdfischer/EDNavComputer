#ifndef EDJOURNAL_H
#define EDJOURNAL_H

#include <QObject>
#include <QThread>
#include "gamestate.h"
#include "location.h"
#include "exploration.h"

class EDJournal : public QObject
{
    Q_OBJECT
    Q_PROPERTY(GameState* gameState READ gameState CONSTANT);
    Q_PROPERTY(Location* location READ location CONSTANT);
    Q_PROPERTY(Exploration* exploration READ exploration CONSTANT);
public:
    explicit EDJournal(QObject *parent = nullptr);
    ~EDJournal();
    GameState* gameState() const;
    Location* location() const;
    Exploration* exploration() const;

public slots:
    void load();

signals:
private:
    GameState* m_gameState;
    Location* m_location;
    Exploration* m_exploration;
    QThread m_loadThread;
};

#endif // EDJOURNAL_H
