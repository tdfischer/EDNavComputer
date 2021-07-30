#ifndef EXPLORATION_H
#define EXPLORATION_H

#include <QObject>
#include "systembodies.h"

class Exploration : public QObject
{
    Q_OBJECT
    Q_PROPERTY(SystemBodies* bodies READ bodies CONSTANT);
public:
    explicit Exploration(QObject *parent = nullptr);
    SystemBodies* bodies() const;

public slots:
    void processJournalEntry(const JournalEntry& jnl);

signals:

private:
    SystemBodies* m_bodies;
};

#endif // EXPLORATION_H
