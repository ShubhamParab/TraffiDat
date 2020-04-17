#include "mutroadtablemodel.h"


MutRoadTableModel::MutRoadTableModel(QList<mut_road> mutRds, QObject *parent):
    QAbstractTableModel (parent),
    mutRds(mutRds)
{

}

int MutRoadTableModel::rowCount(const QModelIndex &parent) const
{
    return mutRds.size();
}

int MutRoadTableModel::columnCount(const QModelIndex &parent) const
{
    return 5;
}

QVariant MutRoadTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    if(index.row() >= mutRds.size() || index.row() < 0)
        return QVariant();
    if(role == Qt::DisplayRole){
        mut_road each = mutRds.at(index.row());
        switch (index.column()) {
        case 0:
            return each.id;
        case 1:
            return each.vehicle_count;
        case 2:
            return each.vehicle_speed;
        case 3:
            return each.congestion_count;
        case 4:
            return each.timestamp;
        default:
            return QVariant("INvALID");
        }
    }
    return QVariant();
}

QVariant MutRoadTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();
    if(orientation == Qt::Horizontal){
        switch (section) {
        case 0:
            return tr("ID");
        case 1:
            return tr("VEHICLE COUNT");
        case 2:
            return tr("VEHICLE SPEED");
        case 3:
            return tr("CONGESTION COUNT");
        case 4:
            return tr("TIMESTAMP");
        default:
            return tr("DEFAULT");
        }
    }
    return QVariant();
}

Qt::ItemFlags MutRoadTableModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::ItemIsEnabled;
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}
