#include "gamestate.h"

GameState::GameState(QObject *parent) : QObject(parent)
{

}

QString
GameState::commanderName() const
{
    return m_commanderName;
}

QString
GameState::systemName() const
{
    return m_systemName;
}

long
GameState::totalJumpDistance() const
{
    return m_totalJumpDistance;
}

void
GameState::processJournalEntry(const JournalEntry& jnl)
{
    switch(jnl.eventType()) {
        case JournalEntry::Commander:
            m_commanderName = jnl.getValue("Name").toString();emit commanderNameUpdate(m_commanderName);break;
        case JournalEntry::LoadGame:
            m_commanderName = jnl.getValue("Commander").toString();emit commanderNameUpdate(m_commanderName);break;
        case JournalEntry::Location:
            m_systemName = jnl.getValue("StarSystem").toString();emit systemNameUpdate(m_systemName);break;
        case JournalEntry::FSDJump:
            m_systemName = jnl.getValue("StarSystem").toString();
            m_totalJumpDistance += jnl.getValue("JumpDist").toDouble();
            emit systemNameUpdate(m_systemName);
            emit totalJumpDistanceUpdate(m_totalJumpDistance);
            break;
        //default:
            //qDebug() << "Unhandled event" << jnl;
    }
}
