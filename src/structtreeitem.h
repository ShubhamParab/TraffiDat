#ifndef STRUCTTREEITEM_H
#define STRUCTTREEITEM_H

#include <QVariant>
#include <QPointer>

#include "model.h"
#include "constants.h"

using namespace TraffiDat;
class StructTreeItem
{
public:
    StructTreeItem(int itemType, StructTreeItem *parentItem = nullptr, junction j = junction(-1), traffic_light tl = traffic_light(), road r = road());
    ~StructTreeItem();

    qint64 getItemType();

    junction getJunct();
    road getRoad();
    traffic_light getTraffL();

    void appendChild(StructTreeItem *item);
    void removeChild(StructTreeItem *item);
    void removeAllChild();

    StructTreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column = 0) const;

    int row() const;
    StructTreeItem *getParentItem();

private:
    qint64 itemType;
    QList<StructTreeItem*> childItems;
    StructTreeItem *parentItem = nullptr;
    junction j;
    road r;
    traffic_light tl;
};

#endif // STRUCTTREEITEM_H
