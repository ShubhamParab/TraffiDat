#include "structtreemodel.h"

StructTreeModel::StructTreeModel(Database *db, QObject *parent):QAbstractItemModel(parent)
{
    this->db = db;
    connect(db, SIGNAL(dbChanged()), this, SLOT(refresh()));
    rootItem = new StructTreeItem(ROOT_ITEM);
    setupModelData(rootItem);
}

//StructTreeModel::~StructTreeModel()
//{
//    if(db != nullptr)
//        delete db;
//    if(rootItem != nullptr)
//        delete rootItem;
//}

QVariant StructTreeModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    if(role != Qt::DisplayRole)
        return QVariant();
    StructTreeItem *item = static_cast<StructTreeItem*>(index.internalPointer());
    return item->data();
}

Qt::ItemFlags StructTreeModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return nullptr;
    return QAbstractItemModel::flags(index);
}

QVariant StructTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole){
        rootItem->data();
    }
    return QVariant();
}

QModelIndex StructTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent))
        return QModelIndex();
    StructTreeItem *parentItem;
    if(!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<StructTreeItem*>(parent.internalPointer());

    StructTreeItem *childItem = parentItem->child(row);
    if(childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex StructTreeModel::parent(const QModelIndex &index) const
{
    if(!index.isValid())
        return QModelIndex();
    StructTreeItem *childItem = static_cast<StructTreeItem*>(index.internalPointer());
    StructTreeItem *parentItem = childItem->getParentItem();
    if(parentItem == rootItem)
        return QModelIndex();
    return createIndex(parentItem->row(), 0, parentItem);
}

int StructTreeModel::rowCount(const QModelIndex &parent) const
{
    StructTreeItem *parentItem;
    if(parent.column() > 0)
        return 0;
    if(!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<StructTreeItem*>(parent.internalPointer());
    return parentItem->childCount();
}

int StructTreeModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return static_cast<StructTreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

void StructTreeModel::refresh()
{
    qDebug() << "Refreshing";
    rootItem->removeAllChild();
    setupModelData(rootItem);
    emit layoutChanged();
}

void StructTreeModel::setupModelData(StructTreeItem *parent)
{
    QList<qint64> rdsInTl;
    for(junction j : db->getAvailJuncts()){
        StructTreeItem *jItem = new StructTreeItem(JUNCTION_ITEM, parent, j);
        parent->appendChild(jItem);
        QList<qint64> upRds = Database::stringToIds(j.up_road_ids);
        QList<qint64> downRds = Database::stringToIds(j.down_road_ids);
        for(traffic_light tl : db->trafficLightsByJunctionId(j.junction_id)){
            StructTreeItem *tlItem = new StructTreeItem(TRAFFIC_LIGHT_ITEM, jItem, j, tl);
            jItem->appendChild(tlItem);
            StructTreeItem *headUpRdsItem = new StructTreeItem(HEAD_UPSTREAM_ROADS_ITEM, tlItem);
            tlItem->appendChild(headUpRdsItem);
            StructTreeItem *headDownRdsItem = new StructTreeItem(HEAD_DOWNSTREAM_ROAD_ITEM, tlItem);
            tlItem->appendChild(headDownRdsItem);
            for(qint64 r : Database::stringToIds(tl.road_ids)){
                road rd = db->roadById(r);
                rdsInTl.append(r);
                if(upRds.contains(r)){
//                    upRds.removeAll(r);
                    StructTreeItem *rdItem = new StructTreeItem(UPSTREAM_ROAD_ITEM, headUpRdsItem, j, tl, rd);
                    headUpRdsItem->appendChild(rdItem);
                }else if(downRds.contains(r)){
//                    downRds.removeAll(r);
                    StructTreeItem *rdItem = new StructTreeItem(DOWNSTREAM_ROAD_ITEM, headDownRdsItem, j, tl, rd);
                    headDownRdsItem->appendChild(rdItem);
                }
            }
        }

        // removing rds which are inside tls
        for(qint64 i : rdsInTl){
            if(upRds.contains(i)){
                upRds.removeAll(i);
            }
            if(downRds.contains(i)){
                downRds.removeAll(i);
            }
        }

        // EXCEPTION: Remaining roads
        if(upRds.size() > 0 || downRds.size() > 0){
            traffic_light freeTl;
            freeTl.traffic_light_id = -1;
            StructTreeItem *freeTlItem = new StructTreeItem(TRAFFIC_LIGHT_ITEM, jItem, j, freeTl);
            jItem->appendChild(freeTlItem);
            StructTreeItem *headUpRdsItem = new StructTreeItem(HEAD_UPSTREAM_ROADS_ITEM, freeTlItem);
            freeTlItem->appendChild(headUpRdsItem);
            StructTreeItem *headDownRdsItem = new StructTreeItem(HEAD_DOWNSTREAM_ROAD_ITEM, freeTlItem);
            freeTlItem->appendChild(headDownRdsItem);
            for(int i : upRds){
                road r = db->roadById(i);
                StructTreeItem *rdItem = new StructTreeItem(UPSTREAM_ROAD_ITEM, headUpRdsItem, j, freeTl, r);
                headUpRdsItem->appendChild(rdItem);
            }
            for(int i : downRds){
                road r = db->roadById(i);
                StructTreeItem *rdItem = new StructTreeItem(DOWNSTREAM_ROAD_ITEM, headDownRdsItem, j, freeTl, r);
                headDownRdsItem->appendChild(rdItem);
            }
        }
    }
}
