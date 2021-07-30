#ifndef JOURNALENTRY_H
#define JOURNALENTRY_H

#include <QJsonObject>

class JournalEntry
{
    Q_GADGET
public:
    typedef enum EventType {
        Unknown,
        Fileheader,
        // Startup
        Cargo,
        ClearSavedGame,
        Commander,
        Loadout,
        Materials,
        Missions,
        NewCommander,
        LoadGame,
        Passengers,
        Powerplay,
        Progress,
        Rank,
        Reputation,
        Statistics,

        // Travel
        ApproachBody,
        Docked,
        DockingCancelled,
        DockingDenied,
        DockingGranted,
        DockingRequested,
        DockingTimeout,
        FSDJump,
        FSDTarget,
        LeaveBody,
        Liftoff,
        Location,
        StartJump,
        SupercruiseEntry,
        SupercruiseExit,
        Touchdown,
        Undocked,
        NavRoute,

        // Combat
        Bounty,
        CapShipBond,
        Died,
        EscapeInterdiction,
        FactionKillBond,
        FighterDestroyed,
        HeatDamage,
        HeatWarning,
        HullDamage,
        Interdicted,
        Interdiction,
        PVPKill,
        ShieldState,
        ShipTargetted,
        SRVDestroyed,
        UnderAttack,

        // Exploration
        CodexEntry,
        DiscoveryScan,
        Scan,
        FSSAllBodiesFound,
        FSSDiscoveryScan,
        FSSSignalDiscovered,
        MaterialCollected,
        MaterialDiscarded,
        MaterialDiscovered,
        MultiSellExplorationData,
        NavBeaconScan,
        BuyExplorationData,
        SAAScanComplete,
        SAASignalsFound,
        SellExplorationData,
        Screenshot,
        // Trade
        // misc
        CommanderName,
        EngineerProgress,
        ReceiveText,
    } EventType;
    Q_ENUM(EventType)
    JournalEntry(const QJsonObject &obj);
    EventType eventType() const;
    QVariant getValue(const QString& key) const;
    QString getString(const QString& key) const;
    QJsonObject getJson() const;

private:
    EventType m_type = Unknown;
    QJsonObject m_json;
};

QDebug operator<<(QDebug debug, const JournalEntry& jnl);

#endif // JOURNALENTRY_H
