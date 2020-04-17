#include "structtreeitem.h"

StructTreeItem::StructTreeItem(int itemType, StructTreeItem *parentItem, junction j, traffic_light tl, road r)
{
    this->itemType = itemType;
    this->parentItem = parentItem;
    this->j = j;
    this->tl = tl;
    this->r = r;
}

StructTreeItem::~StructTreeItem()
{
    qDeleteAll(childItems);
//    if(parentItem != nullptr)
//        delete parentItem;
}

qint64 StructTreeItem::getItemType()
{
    return itemType;
}

junction StructTreeItem::getJunct()
{
    return j;
}

road StructTreeItem::getRoad()
{
    return r;
}

traffic_light StructTreeItem::getTraffL()
{
    return tl;
}

void StructTreeItem::appendChild(StructTreeItem *item)
{
    childItems.append(item);
}

void StructTreeItem::removeChild(StructTreeItem *item)
{
    childItems.removeAll(item);
}

void StructTreeItem::removeAllChild()
{
    childItems.clear();
}

StructTreeItem *StructTreeItem::child(int row)
{
    return childItems.value(row);
}

int StructTreeItem::childCount() const
{
    return childItems.count();
}

int StructTreeItem::columnCount() const
{
    return 1;
}

QVariant StructTreeItem::data(int column) const
{
    Q_UNUSED(column);
    switch (itemType) {
    case ROOT_ITEM:
        return QString("Junctions: ");
    case JUNCTION_ITEM:
        return QString("JUNCTION: %1").arg(j.name);
    case TRAFFIC_LIGHT_ITEM:
        if(tl.traffic_light_id == -1)
            return QString("FREE FLOW");
        return QString("TRAFFIC LIGHT: %1").arg(tl.traffic_light_id);
    case HEAD_UPSTREAM_ROADS_ITEM:
        return QString("UPSTREAM ROADS");
    case UPSTREAM_ROAD_ITEM:
        return QString("ROAD: %1").arg(r.name);
    case HEAD_DOWNSTREAM_ROAD_ITEM:
        return QString("DOWNSTREAM ROADS");
    case DOWNSTREAM_ROAD_ITEM:
        return QString("ROAD: %1").arg(r.name);
    default:
        return QString("INVALID DATA");
    }
}

int StructTreeItem::row() const
{
    if(parentItem){
        parentItem->childItems.indexOf(const_cast<StructTreeItem*>(this));
    }
    return 0;
}

StructTreeItem *StructTreeItem::getParentItem()
{
    return parentItem;
}
