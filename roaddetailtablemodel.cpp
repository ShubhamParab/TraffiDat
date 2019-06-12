#include "roaddetailtablemodel.h"

RoadDetailTableModel::RoadDetailTableModel(MapModel* model, QObject *parent):
    QAbstractTableModel (parent),
    model(model)
{

}

int RoadDetailTableModel::rowCount(const QModelIndex &parent) const
{
    return model->getAllRds().values().size();
}

// ROAD_NAME | ROAD_LENGTH | ROAD_LANES | PERC_DAMAGE | AVG_VEHICLE_COUNT | AVG_VEHICLE_SPEED | AVG_CONGESTION_COUNT
int RoadDetailTableModel::columnCount(const QModelIndex &parent) const
{
    return 7;
}

QVariant RoadDetailTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    if(index.row() >= model->getAllRds().size() || index.row() < 0)
        return QVariant();

    junction j = model->getJunction();
    road r = model->getAllRds().values().at(index.row());
    mut_road mr = *model->getMutRds().values().at(index.row());
    float p = *model->getTrafficBreakProbabs().values().at(index.row());

    QList<traffic_light>* tl = model->getTlByRoadId(r.road_id);
    QList<traffic_light>* analytl = model->getAnalyTlByRoadId(r.road_id);

    if(role == Qt::DisplayRole){
        switch (index.column()) {
        case 0:
            return r.name;
        case 1:
            return QString::number(r.length);
        case 2:
            return QString::number(r.lanes);
        case 3:
            return QString("%1 %").arg(QString::number(r.perc_damage));
        case 4:
            return QString::number(mr.vehicle_count);
        case 5:
            return QString::number(mr.vehicle_speed);
        case 6:
            return QString::number(mr.congestion_count*100);
        default:
            return QVariant("INvALID");
        }
    }
    return QVariant();
}

QVariant RoadDetailTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();
    if(orientation == Qt::Horizontal){
        switch(section){
        case 0:
            return QString("NAME");
        case 1:
            return QString("LENGTH (meters)");
        case 2:
            return QString("LANES");
        case 3:
            return QString("PERC_DAMAGE");
        case 4:
            return QString("VEHICLE COUNT");
        case 5:
            return QString("VEHICLE SPEED (km/hr)");
        case 6:
            return QString("CONGESTION COUNT");
        default:
            return QString("DEFAULT");
        }
    }else if(orientation == Qt::Vertical){
        junction j = model->getJunction();
        road r = model->getAllRds().values().at(section);
        if(model->getDatabase()->stringToIds(j.up_road_ids).contains(r.road_id)){
            return QString("UPSTREAM ROAD");
        }else if(model->getDatabase()->stringToIds(j.down_road_ids).contains(r.road_id)){
            return QString("DOWNSTREAM ROAD");
        }else{
            return QString("NO ALLOC ROAD");
        }
    }
    return QVariant();
}

Qt::ItemFlags RoadDetailTableModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::ItemIsEnabled;
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

Road2TraffLChangeTableModel::Road2TraffLChangeTableModel(MapModel *model, QObject *parent):
    QAbstractTableModel (parent),
    model(model)
{

}

int Road2TraffLChangeTableModel::rowCount(const QModelIndex &parent) const
{
    return model->getMutRds().values().size();
}

// ROAD_ID | ROAD_NAME | ORIGINAL_TRAFFL | CURRENT_TRAFFL
int Road2TraffLChangeTableModel::columnCount(const QModelIndex &parent) const
{
    return 4;
}

QVariant Road2TraffLChangeTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    if(index.row() >= model->getAllRds().size() || index.row() < 0)
        return QVariant();

    junction j = model->getJunction();
    road r = model->getAllRds().values().at(index.row());
    mut_road mr = *model->getMutRds().values().at(index.row());
    float p = *model->getTrafficBreakProbabs().values().at(index.row());

    QList<traffic_light>* tl = model->getTlByRoadId(r.road_id);
    QList<traffic_light>* analytl = model->getAnalyTlByRoadId(r.road_id);

    if(role == Qt::DisplayRole){
        switch (index.column()) {
        case 0:
            return QString::number(r.road_id);
        case 1:
            return r.name;
        case 2:
            return QString("TRAFFLS: %1").arg(repr_traffLs(*tl));
        case 3:
            return QString("TRAFFLS: %1").arg(repr_traffLs(*analytl));
        default:
            return QVariant("INvALID");
        }
    }
    return QVariant();
}

QVariant Road2TraffLChangeTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();
    if(orientation == Qt::Horizontal){
        switch (section) {
        case 0:
            return tr("ROAD ID");
        case 1:
            return tr("ROAD NAME");
        case 2:
            return tr("ORIGINAL TRAFFL");
        case 3:
            return tr("CURRENT TRAFFL");
        default:
            return tr("DEFAULT");
        }
    }
    return QVariant();
}

Qt::ItemFlags Road2TraffLChangeTableModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::ItemIsEnabled;
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

TraffL2TimerChangeTableModel::TraffL2TimerChangeTableModel(MapModel *model, QObject *parent):
    QAbstractTableModel (parent),
    model(model)
{
    oldTls = model->getTraffLsByRd2TraffL(model->getTls());
    newTls = model->getTraffLsByRd2TraffL(model->getAnalyTls());

    qDebug() << "oldTls: " << oldTls.size();
    qDebug() << "newTls: " << newTls.size();

    // building tl category
    for(qint64 i : newTls.keys()){
        if(oldTls.value(i)->road_ids == "" && newTls.value(i)->road_ids != ""){
            catTls.insert(i, TlCategory::NEW_TL);
        }else if(oldTls.value(i)->road_ids != "" && newTls.value(i)->road_ids == ""){
            catTls.insert(i, TlCategory::UNAVAILABLE_TL);
        }else if(oldTls.value(i)->road_ids != "" && newTls.value(i)->road_ids != ""){
            catTls.insert(i, TlCategory::UPDATED_TL);
        }else if(oldTls.value(i)->road_ids == "" && newTls.value(i)->road_ids == ""){
            catTls.insert(i, TlCategory::UNUSED_TL);
        }
    }

    qDebug() << "catTls: " << catTls.size();
}

int TraffL2TimerChangeTableModel::rowCount(const QModelIndex &parent) const
{
    return newTls.size();
}

// TRAFFL | ORIGINAL TIMER | CURRENT TIMER | USE_COUNT
int TraffL2TimerChangeTableModel::columnCount(const QModelIndex &parent) const
{
    return 4;
}

QVariant TraffL2TimerChangeTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    if(index.row() >= newTls.size() || index.row() < 0)
        return QVariant();

    traffic_light* oldTl = oldTls.values().at(index.row());
    traffic_light* newTl = newTls.values().at(index.row());

    mut_traffic_light* oldMutTl = model->getMutTraffLById(oldTl->traffic_light_id);
    mut_traffic_light* newMutTl = new mut_traffic_light(newTl->traffic_light_id, *model->getTimerTls().value(newTl->traffic_light_id));

    if(role == Qt::DisplayRole){
        switch (index.column()) {
        case 0:
            if(oldTl->traffic_light_id == newTl->traffic_light_id)
                return QString::number(oldTl->traffic_light_id);
            else
                return QString::number(-1);
        case 1:
            return QString::number(oldMutTl->green_timer);
        case 2:
            return QString::number(newMutTl->green_timer);
        case 3:
            return QString::number(oldMutTl->use_count);
        default:
            return QVariant("INvALID");
        }
    }
    return QVariant();
}

QVariant TraffL2TimerChangeTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();
    if(orientation == Qt::Horizontal){
        switch (section) {
        case 0:
            return tr("TRAFFL");
        case 1:
            return tr("ORIGINAL TIMER");
        case 2:
            return tr("CURRENT TIMER (sec)");
        case 3:
            return tr("USE COUNT");
        default:
            return tr("DEFAULT");
        }
    }
    if(orientation == Qt::Vertical){
        switch (catTls.values().at(section)) {
        case NEW_TL:
            return tr("NEW TL");
        case UNUSED_TL:
            return tr("UNUSED TL");
        case UPDATED_TL:
            return tr("UPDATED TL");
        case UNAVAILABLE_TL:
            return tr("UNAVAILABLE TL");
        default:
            return tr("DEFAULT TL");
        }
    }
    return QVariant();
}

Qt::ItemFlags TraffL2TimerChangeTableModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::ItemIsEnabled;
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}
