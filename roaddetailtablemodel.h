#ifndef ROADDETAILTABLEMODEL_H
#define ROADDETAILTABLEMODEL_H

#include <QAbstractTableModel>
#include <QPointer>

#include "mapmodel.h"

class RoadDetailTableModel: public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit RoadDetailTableModel(MapModel* model, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent)const override;
    int columnCount(const QModelIndex& parent)const override;
    QVariant data(const QModelIndex& index, int role)const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role)const override;
    Qt::ItemFlags flags(const QModelIndex& index)const override;

private:
    QPointer<MapModel> model;

};

class Road2TraffLChangeTableModel: public QAbstractTableModel{
    Q_OBJECT
public:
    explicit Road2TraffLChangeTableModel(MapModel* model, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent)const override;
    int columnCount(const QModelIndex& parent)const override;
    QVariant data(const QModelIndex& index, int role)const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role)const override;
    Qt::ItemFlags flags(const QModelIndex& index)const override;
private:
    QPointer<MapModel> model;
};

class TraffL2TimerChangeTableModel: public QAbstractTableModel{
    Q_OBJECT
public:
    explicit TraffL2TimerChangeTableModel(MapModel* model, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent)const override;
    int columnCount(const QModelIndex& parent)const override;
    QVariant data(const QModelIndex& index, int role)const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role)const override;
    Qt::ItemFlags flags(const QModelIndex& index)const override;

private:
    QPointer<MapModel> model;

    QMap<qint64, traffic_light*> oldTls;
    QMap<qint64, traffic_light*> newTls;

    QMap<qint64, TlCategory> catTls;
};

#endif // ROADDETAILTABLEMODEL_H
