#include "location.h"
#include "journalentry.h"

Location::Location(QObject *parent) : QObject(parent)
{
}

void
Location::processJournalEntry(const JournalEntry& jnl)
{
    switch(jnl.eventType()) {
        case JournalEntry::Location:
            m_system = jnl.getString("StarSystem");
            m_body = jnl.getString("Body");
            m_station = jnl.getString("StationName");
            emit stationChanged(m_station);
            emit systemChanged(m_system);
            emit bodyChanged(m_body);
            break;
        case JournalEntry::ApproachBody:
            m_body = jnl.getString("Body");
            emit bodyChanged(m_body);
            break;
        case JournalEntry::FSDJump:
            m_system = jnl.getString("StarSystem");
            m_body = jnl.getString("Body");
            emit systemChanged(m_system);
            emit bodyChanged(m_body);
            break;
        case JournalEntry::SupercruiseExit:
            m_body = jnl.getString("Body");
            emit bodyChanged(m_body);
            break;
        case JournalEntry::Docked:
            m_station = jnl.getString("StationName");
            emit stationChanged(m_station);
            break;
    }
}

QString
Location::system() const
{
    return m_system;
}

QString
Location::body() const
{
    return m_body;
}

QString
Location::station() const
{
    return m_station;
}
