#ifndef JOURNALLOADER_H
#define JOURNALLOADER_H

#include <QObject>
#include "journalentry.h"

class JournalLoader : public QObject
{
    Q_OBJECT
public:
    explicit JournalLoader(QObject *parent = nullptr);
public slots:
    void load();

signals:
    void newEvent(const JournalEntry& entry);
    void finished();

};

#endif // JOURNALLOADER_H
