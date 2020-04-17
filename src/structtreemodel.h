#ifndef STRUCTTREEMODEL_H
#define STRUCTTREEMODEL_H

#include <QAbstractItemModel>
#include <QPointer>

#include "constants.h"
#include "model.h"
#include "database.h"
#include "structtreeitem.h"

using namespace TraffiDat;
class StructTreeModel: public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit StructTreeModel(Database *db, QObject *parent = nullptr);
    // ~StructTreeModel()override;

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

public slots:
    void refresh();

private:
    void setupModelData(StructTreeItem *parent);
    Database *db = nullptr;
    StructTreeItem *rootItem = nullptr;
};

#endif // STRUCTTREEMODEL_H
