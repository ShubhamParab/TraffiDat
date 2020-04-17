#ifndef MAPGRAPHICSSCENE_H
#define MAPGRAPHICSSCENE_H
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QPointer>
#include <QMouseEvent>
#include <QObject>

#include "mapmodel.h"
#include "model.h"
#include "constants.h"

class MapGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    MapGraphicsScene(MapModel *model, QObject *parent = nullptr);
    void build();
    void updateStatsText();
    void setSecTime(qint64 value){secTime = value;}
    qint64 getSecTime(){return secTime;}
    ~MapGraphicsScene();

public slots:
    void updateTime(qint64 updateTime = 1);

private:
    MapModel *model;
    qint64 secTime = 0;
    QPointer<QGraphicsTextItem> statsText;
};

#endif // MAPGRAPHICSSCENE_H
