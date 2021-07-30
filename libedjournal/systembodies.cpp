#include "systembodies.h"

SystemBodies::SystemBodies(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QHash<int, QByteArray>
SystemBodies::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames[Name] = "name";
    roleNames[ID] = "id";
    roleNames[StarType] = "starType";
    roleNames[PlanetClass] = "planetClass";
    roleNames[BodyType] = "bodyType";
    roleNames[ScanValue] = "scanValue";
    return roleNames;
}

QVariant SystemBodies::headerData(int section, Qt::Orientation orientation, int role) const
{
    return "Body";
}

QModelIndex SystemBodies::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return createIndex(row, column, nullptr);
    }
    return QModelIndex();
}

QModelIndex SystemBodies::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int SystemBodies::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return m_bodies.size();

    return 0;
}

int SystemBodies::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 1;

    return 0;
}

QVariant SystemBodies::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const Body& body = m_bodies[index.row()];

    switch((ColumnName)role) {
        case Name:
            return body.name;
        case ID:
            return body.id;
        case StarType:
            return body.starType;
        case PlanetClass:
            return body.planetClass;
        case BodyType:
            return body.type;
        case ScanValue:
            return body.scanValue();
    }

    // FIXME: Implement me!
    return QVariant();
}

void
SystemBodies::processScan(const JournalEntry& jnl)
{
    Body foundBody;
    foundBody.id = jnl.getValue("BodyID").toInt();
    foundBody.name = jnl.getString("BodyName");
    foundBody.starType = jnl.getString("StarType");
    foundBody.planetClass = jnl.getString("PlanetClass");
    foundBody.arrivalDistance = jnl.getValue("DistanceFromArrivalLS").toInt();
    foundBody.isMapped = jnl.getValue("WasMapped").toBool();
    foundBody.isFirstDiscoverer = !jnl.getValue("WasDiscovered").toBool();
    if (!foundBody.starType.isEmpty()) {
        foundBody.type = Star;
        if (foundBody.starType == "N") {
            foundBody.type = NeutronStar;
        }
        foundBody.mass = jnl.getValue("StellarMass").toDouble();
    } else if (!foundBody.planetClass.isEmpty()) {
        foundBody.type = Planet;
        foundBody.landable = jnl.getValue("Landable").toBool();
        if (foundBody.planetClass.contains("class I gas giant")) {
            foundBody.type = GasGiant1;
        } else if (foundBody.planetClass.contains("class II gas giant")) {
            foundBody.type = GasGiant2;
        } else if (foundBody.planetClass.contains("High metal content")) {
            foundBody.type = HighMetalContent;
        } else if (foundBody.planetClass.contains("Metal rich body")) {
            foundBody.type = MetalRich;
        } else if (foundBody.planetClass.contains("ammonia")) {
            foundBody.type = AmmoniaWorld;
        } else if (foundBody.planetClass.contains("Earth-Like")) {
            foundBody.type = EarthLikeWorld;
        }
        foundBody.mass = jnl.getValue("MassEM").toDouble();
    } else if (foundBody.name.contains("Belt Cluster")) {
        foundBody.type = BeltCluster;
    } else {
        foundBody.type = Unknown;
    }
    beginInsertRows(QModelIndex(), m_bodies.size(), m_bodies.size());
    m_bodies.append(foundBody);
    endInsertRows();
}

void
SystemBodies::reset()
{
    beginResetModel();
    m_bodies.clear();
    endResetModel();
}

// Scan values and formula taken from:
// https://forums.frontier.co.uk/threads/exploration-value-formulae.232000/
int
SystemBodies::Body::baseScanValue() const
{
    switch(type) {
        case Star:
            return 1200;
        case BlackHole:
        case NeutronStar:
            return 22628;
        case WhiteDwarf:
            return 14057;
        case MetalRich:
            return 21790;
        case AmmoniaWorld:
            return 96932;
        case GasGiant1:
            return 1656;
        case GasGiant2:
        case HighMetalContent:
            return terraformable ? 100677 : 9654;
        case WaterWorld:
        case EarthLikeWorld:
            return terraformable ? 116295 : 64831;
        default:
            return terraformable ? 93328 : 300;
    }
}

int
SystemBodies::Body::scanValue() const
{
    const double q = 0.56591828;
    double mappingMultiplier = 1;
    int k = baseScanValue();

    if(isMapped) {
        if(isFirstDiscoverer && isFirstMapped)
        {
            mappingMultiplier = 3.699622554;
        }
        else if(isFirstMapped)
        {
            mappingMultiplier = 8.0956;
        }
        else
        {
            mappingMultiplier = 3.3333333333;
        }
    }
    double value = (k + k * q * pow(mass,0.2)) * mappingMultiplier;
    if(isMapped) {
        if(landable)
        {
            value += ((value * 0.3) > 555) ? value * 0.3 : 555;
        }
        /*if(withEfficiencyBonus)
        {
            value *= 1.25;
        }*/
    }
    value = qMax((double)500, value);
    value *= (isFirstDiscoverer) ? 2.6 : 1;
    //value *= (isFleetCarrierSale) ? 0.75 : 1;
    return round(value);
}
