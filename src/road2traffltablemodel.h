#ifndef ROAD2TRAFFLTABLEMODEL_H
#define ROAD2TRAFFLTABLEMODEL_H

#include <QAbstractTableModel>
#include <QPointer>

#include "mapmodel.h"
#include "model.h"

using namespace TraffiDat;
class Road2TraffLTableModel: public QAbstractTableModel
{
public:
    Road2TraffLTableModel(MapModel* model, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent)const override;
    int columnCount(const QModelIndex& parent)const override;
    QVariant data(const QModelIndex& index, int role)const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role)const override;
    Qt::ItemFlags flags(const QModelIndex& index)const override;

    ~Road2TraffLTableModel();

private:
    QPointer<MapModel> model;

};

#endif // ROAD2TRAFFLTABLEMODEL_H
