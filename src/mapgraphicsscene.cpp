#include "mapgraphicsscene.h"

MapGraphicsScene::MapGraphicsScene(MapModel *model, QObject *parent):
    QGraphicsScene (parent),
    model(model)
{
    statsText = new QGraphicsTextItem("STATS : ");
    statsText->setFont(QFont("Helvetica [Cronyx]", 1));
    build();
}

void MapGraphicsScene::build()
{
    if(model->getMapType() == ROAD_MAP_TYPE){
        if(model->getRoadGraphicsItem() != nullptr)
            addItem(model->getRoadGraphicsItem());
    }else if(model->getMapType() == JUNCTION_MAP_TYPE){
        if(model->getRoadsGraphicsItem().size() > 0){
            for(RoadGraphicsItem* i : model->getRoadsGraphicsItem()){
                addItem(i);
            }
        }
    }else{
        // soon
    }
    if(model->getJunctionGraphicsItem() != nullptr)
        addItem(model->getJunctionGraphicsItem());

    addItem(statsText);

    setSceneRect(itemsBoundingRect());
    if(model->getMapType() == JUNCTION_MAP_TYPE){
        statsText->setPos(sceneRect().x(), sceneRect().y());
    }else if(model->getMapType() == ROAD_MAP_TYPE){
        statsText->setPos(sceneRect().x()+sceneRect().width()-40, sceneRect().y()+sceneRect().height()-20);
    }
    update();
}

void MapGraphicsScene::updateStatsText()
{
    if(model->getAllRds().size() > 0){
        statsText->setPlainText(QString("STATS : \nTIME: %1 sec\nNumber of Roads: %2\nNumber of Traffic phases: %3\nCurrent Phase ID: %4").arg(secTime).arg(model->getAllRds().size()).arg(model->getTrffToRds().uniqueKeys().size()).arg(model->getPhase()));
    }
}

MapGraphicsScene::~MapGraphicsScene()
{
//    if(model != nullptr)
    //        delete model;
}

void MapGraphicsScene::updateTime(qint64 updateTime)
{
    secTime = model->getTime();
    updateStatsText();
}

