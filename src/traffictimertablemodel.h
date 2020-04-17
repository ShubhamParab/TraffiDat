#ifndef TRAFFICTIMERTABLEMODEL_H
#define TRAFFICTIMERTABLEMODEL_H

#include <QAbstractTableModel>
#include <QPointer>

#include "mapmodel.h"

class TrafficTimerTableModel: public QAbstractTableModel
{
public:
    TrafficTimerTableModel(MapModel* model, QMap<qint64, TlCategory> catTls, QMap<qint64, mut_traffic_light> mutTls, QMap<qint64, qint64*> &timerTls, qint64 mean_use_count = 0, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent)const override;
    int columnCount(const QModelIndex& parent)const override;
    QVariant data(const QModelIndex& index, int role)const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role)const override;
    Qt::ItemFlags flags(const QModelIndex& index)const override;

    ~TrafficTimerTableModel();

private:
    QPointer<MapModel> model;

    QMap<qint64, traffic_light*> oldTls;
    QMap<qint64, traffic_light*> newTls;
    QMap<qint64, mut_traffic_light> mutTls;
    QMap<qint64, qint64*> timerTls;
    QMap<qint64, TlCategory> catTls;
    qint64 mean_use_count = 0;
};

#endif // TRAFFICTIMERTABLEMODEL_H
