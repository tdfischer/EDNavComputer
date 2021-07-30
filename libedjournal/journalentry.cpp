#include "journalentry.h"
#include <QMetaEnum>

JournalEntry::JournalEntry(const QJsonObject &obj)
    : m_json(obj)
{
    QString eventName = obj["event"].toString();
    bool ok;
    m_type = (EventType)QMetaEnum::fromType<EventType>().keyToValue(eventName.toLocal8Bit().data(), &ok);
    if (!ok) {
        m_type = Unknown;
    }
}

QJsonObject
JournalEntry::getJson() const
{
    return m_json;
}

QDebug operator<<(QDebug debug, const JournalEntry &jnl)
{
    debug.nospace() << "JournalEntry(" << jnl.eventType() << ")";
    return debug.maybeSpace();
}

JournalEntry::EventType
JournalEntry::eventType() const
{
    return m_type;
}

QVariant
JournalEntry::getValue(const QString& key) const
{
    return m_json[key].toVariant();
}

QString
JournalEntry::getString(const QString& key) const
{
    return getValue(key).toString();
}
