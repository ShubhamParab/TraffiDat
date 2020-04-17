#ifndef MUTROADTABLEMODEL_H
#define MUTROADTABLEMODEL_H

#include <QAbstractTableModel>

#include "model.h"

using namespace TraffiDat;
class MutRoadTableModel:public QAbstractTableModel
{
public:
    MutRoadTableModel(QList<mut_road> mutRds, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent)const override;
    int columnCount(const QModelIndex& parent)const override;
    QVariant data(const QModelIndex& index, int role)const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role)const override;
    Qt::ItemFlags flags(const QModelIndex& index)const override;

    QList<mut_road> getMutRds(){return mutRds;}

private:
    QList<mut_road> mutRds;
};

#endif // MUTROADTABLEMODEL_H
