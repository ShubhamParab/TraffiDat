#include "traffictimertablemodel.h"


TrafficTimerTableModel::TrafficTimerTableModel(MapModel *model, QMap<qint64, TlCategory> catTls, QMap<qint64, mut_traffic_light> mutTls, QMap<qint64, qint64*> &timerTls, qint64 mean_use_count, QObject *parent):
    QAbstractTableModel (parent),
    model(model),
    mutTls(mutTls),
    timerTls(timerTls),
    catTls(catTls),
    mean_use_count(mean_use_count)
{
    qDebug() << "Average use count: " << mean_use_count;
    oldTls = model->getTraffLsByRd2TraffL(model->getTls());
    newTls = model->getTraffLsByRd2TraffL(model->getAnalyTls());
}

int TrafficTimerTableModel::rowCount(const QModelIndex &parent) const
{
    return mutTls.size();
}

int TrafficTimerTableModel::columnCount(const QModelIndex &parent) const
{
    return 6;
}

QVariant TrafficTimerTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    if(index.row() >= mutTls.size() || index.row() < 0)
        return QVariant();

    traffic_light* oldEach = oldTls.values().at(index.row());
    traffic_light* newEach = newTls.values().at(index.row());
    mut_traffic_light mutEach = mutTls.values().at(index.row());
    qint64* timer = timerTls.values().at(index.row());


    if(role == Qt::DisplayRole){
        switch (index.column()) {
        case 0:
            return QString::number(oldEach->traffic_light_id);
        case 1:
            return QString("ROADS: %1").arg(oldEach->road_ids);
        case 2:
            return QString("ROADS: %1").arg(newEach->road_ids);
        case 3:
            return QString::number(mutEach.use_count-mean_use_count);
        case 4:
            return QString::number(mutEach.green_timer);
        case 5:
            return QString::number(*timer);
        default:
            return QString("DEFAULT COL");
        }
    }
    return QVariant();
}

QVariant TrafficTimerTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();
    if(orientation == Qt::Horizontal){
        switch (section) {
        case 0:
            return tr("TRAFFIC ID");
        case 1:
            return tr("EARLIER ROADS");
        case 2:
            return tr("CURRENT ROADS");
        case 3:
            return tr("USE COUNT");
        case 4:
            return tr("EARLIER TIMER");
        case 5:
            return tr("RECOMMENDED TIMER");
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

Qt::ItemFlags TrafficTimerTableModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::ItemIsEnabled;
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

TrafficTimerTableModel::~TrafficTimerTableModel()
{

}
