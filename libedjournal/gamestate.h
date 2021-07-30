#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <QObject>
#include "journalentry.h"

class GameState : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString commanderName READ commanderName NOTIFY commanderNameUpdate);
    Q_PROPERTY(QString systemName READ systemName NOTIFY systemNameUpdate);
    Q_PROPERTY(long totalJumpDistance READ totalJumpDistance NOTIFY totalJumpDistanceUpdate);
public:
    explicit GameState(QObject *parent = nullptr);
    QString commanderName() const;
    QString systemName() const;
    long totalJumpDistance() const;

public slots:
    void processJournalEntry(const JournalEntry& jnl);

signals:
    void commanderNameUpdate(const QString& name);
    void systemNameUpdate(const QString& name);
    void totalJumpDistanceUpdate(long distance);
private:
    QString m_commanderName;
    QString m_systemName;
    long m_totalJumpDistance = 0;
};

#endif // GAMESTATE_H
