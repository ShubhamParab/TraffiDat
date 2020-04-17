#include "road2traffltablemodel.h"


Road2TraffLTableModel::Road2TraffLTableModel(MapModel *model, QObject *parent):
    QAbstractTableModel (parent),
    model(model)
{
//    qDebug() << "SIZEOF ROADS: " << model->getAllRds().size();
//    qDebug() << "SIZEOF MUT_ROAD: " << model->getMutRds().values().size();
//    qDebug() << "SIZEOF MUT_TRAFFIC_LIGHT: " << model->getMutTraffL().values().size();
//    qDebug() << "SIZEOF TRAFFIC_LIGHT: " << model->getTls().values().size();
//    qDebug() << "SIZEOF ANALY_TRAFFIC_LIGHT: " << model->getAnalyTls().values().size();
}

int Road2TraffLTableModel::rowCount(const QModelIndex &parent) const
{
    return model->getAllRds().size();
}

// ROAD ID | ROAD NAME | TRAFFIC FLOW | TRAFFIC FLUX | TRAFFIC SPEED | TRAFFIC DENSITY | CURRENT TRAFFIC LIGHT | RECOMMENDED TRAFFIC LIGHT| View In Graph
int Road2TraffLTableModel::columnCount(const QModelIndex &parent) const
{
    return 9;
}

QVariant Road2TraffLTableModel::data(const QModelIndex &index, int role) const
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
            return r.road_id;
        case 1:
            return r.name;
        case 2:
            if(Database::stringToIds(j.up_road_ids).contains(r.road_id)){
                return tr("UPSTREAM");
            }else if(Database::stringToIds(j.down_road_ids).contains(r.road_id)){
                return tr("DOWNSTREAM");
            }else{
                return tr("INVALID FLOW");
            }
        case 3:
            return QString::number(mr.vehicle_count);
        case 4:
            return QString::number(mr.vehicle_speed);
        case 5:
            return QString::number(p);
        case 6:
            return QString("TRAFFIC IDS :%1").arg(repr_traffLs(*tl));
        case 7:
            return QString("TRAFFIC IDS: %1").arg(repr_traffLs(*analytl));
        case 8:
            return QString("View In Graph");
        default:
            return QVariant("INvALID");
        }
    }
    return QVariant();
}

QVariant Road2TraffLTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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
            return tr("TRAFFIC FLOW (DIRECTION)");
        case 3:
            return tr("TRAFFIC FLUX (cars/min)");
        case 4:
            return tr("TRAFFIC SPEED (km/hr)");
        case 5:
            return tr("TRAFFIC DENSITY (cars/km)");
        case 6:
            return tr("CURRENT TRAFFIC LIGHT");
        case 7:
            return tr("RECOMMENDED TRAFFIC LIGHT");
        case 8:
            return tr("View In Graph");
        default:
            return tr("DEFAULT");
        }
    }
    return QVariant();
}

Qt::ItemFlags Road2TraffLTableModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::ItemIsEnabled;
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

Road2TraffLTableModel::~Road2TraffLTableModel()
{

}
