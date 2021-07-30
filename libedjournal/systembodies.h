#ifndef SYSTEMBODIES_H
#define SYSTEMBODIES_H

#include <QAbstractItemModel>
#include "journalentry.h"

class SystemBodies : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum Type {
        Unknown,
        Planet,
        Star,
        NeutronStar,
        BlackHole,
        WhiteDwarf,
        MetalRich,
        AmmoniaWorld,
        GasGiant1,
        GasGiant2,
        HeliumRichGiant,
        HighMetalContent,
        WaterWorld,
        EarthLikeWorld,
        BeltCluster,
    };
    Q_ENUM(Type);

    enum ColumnName {
        // Common properties
        Name = Qt::UserRole + 1,
        ID,
        ArrivalDistance,
        SurfaceTemperature,
        Rings,
        RotationPeriod,
        SemiMajorAxis,
        Eccentricity,
        OrbitalInclination,
        Periapsis,
        OrbitalPeriod,

        // Calculated properties
        BodyType,
        ScanValue,

        // Star properties
        StarType,
        StarSubclass,
        StellarMass,
        Radius,
        AbsoluteMagnitude,
        Luminosity,
        Age_MY,
        WasDiscovered,
        WasMapped,

        // Planet properties
        TidalLock,
        TerraformState,
        PlanetClass,
        Atmosphere,
        AtmosphereType,
        AtmosphereComposition,
        Volcanism,
        SurfaceGravity,
        SurfacePressure,
        Landable,
        Materials,
        Composition,
        ReserveLevel,
        AxialTilt,

        // Rings
        RingClass,
        MassMT,
        InnerRadius,
        OuterRadius,
    };
    explicit SystemBodies(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const;

    void processScan(const JournalEntry& jnl);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void reset();

private:
    struct Body {
        QString name;
        int id;
        int arrivalDistance;
        QString starType;
        QString planetClass;
        bool isMapped;
        bool isFirstDiscoverer;
        bool isFirstMapped;
        bool terraformable;
        bool landable;
        double mass;
        Type type;

        int scanValue() const;
        int baseScanValue() const;
    };
    QList<Body> m_bodies;
};

#endif // SYSTEMBODIES_H
