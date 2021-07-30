#ifndef LOCATION_H
#define LOCATION_H

#include <QObject>
#include "journalentry.h"

class Location : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString system READ system NOTIFY systemChanged)
    Q_PROPERTY(QString body READ body NOTIFY bodyChanged)
    Q_PROPERTY(QString station READ station NOTIFY stationChanged);
public:
    explicit Location(QObject *parent = nullptr);
    QString system() const;
    QString body() const;
    QString station() const;

public slots:
    void processJournalEntry(const JournalEntry& jnl);

signals:
    void systemChanged(const QString& system);
    void bodyChanged(const QString& body);
    void stationChanged(const QString& station);

private:
    QString m_system;
    QString m_body;
    QString m_station;
};

#endif // LOCATION_H
