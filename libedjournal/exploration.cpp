#include "exploration.h"
#include "journalentry.h"

Exploration::Exploration(QObject *parent) : QObject(parent)
{
    m_bodies = new SystemBodies();
}

SystemBodies*
Exploration::bodies() const
{
    return m_bodies;
}

void
Exploration::processJournalEntry(const JournalEntry& jnl)
{
    switch (jnl.eventType()) {
        case JournalEntry::FSDJump:
            m_bodies->reset();
        case JournalEntry::Scan:
            m_bodies->processScan(jnl);
    }
}
