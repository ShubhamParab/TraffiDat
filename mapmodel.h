#ifndef MAPMODEL_H
#define MAPMODEL_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QPointer>
#include <QTimer>

#include "database.h"
#include "model.h"
#include "constants.h"

#include "junctiongraphicsitem.h"
#include "roadgraphicsitem.h"

using namespace TraffiDat;

enum TlCategory{
    UPDATED_TL,
    NEW_TL,
    UNAVAILABLE_TL,
    UNUSED_TL
};

class MapModel : public QObject
{
    Q_OBJECT
public:
    MapModel(qint64 mapType, Database *db, QObject *parent = nullptr);
    void build(struct junction j, struct road r);
    void buildTrafficLights();
    qint64 getMapType(){return mapType;}
    JunctionGraphicsItem *getJunctionGraphicsItem();
    RoadGraphicsItem *getRoadGraphicsItem();
    QMap<qint64, RoadGraphicsItem*> getRoadsGraphicsItem();
    ~MapModel();

    QList<road> getUpstreamRds(){return upstreamRds;}
    QList<qint64> getUpstreamRdAngles(){return upstreamRdAngles;}
    QList<road> getDownstreamRds(){return downstreamRds;}
    QList<qint64> getDownstreamRdAngles(){return downstreamRdAngles;}
    QMultiMap<qint64, road> getTrffToRds(){return trfflToRds;}

    junction getJunction(){return junction;}
    QMap<qint64, road> getAllRds();
    mut_road* getMutRdById(qint64 id){return mutRds.value(id);}

    QMap<qint64, QList<traffic_light>*>& getTls(){return rd2tls;}
    QList<traffic_light>* getTlByRoadId(qint64 id){return rd2tls.value(id);}

    QMap<qint64, QList<traffic_light>*>& getAnalyTls(){return rd2analy_tls;}
    QList<traffic_light>* getAnalyTlByRoadId(qint64 id){return rd2analy_tls.value(id);}

    QMap<qint64, mut_traffic_light*>& getPreMutTraffL(){return preMutTl;}
    mut_traffic_light* getPreMutTraffLById(qint64 id){return preMutTl.value(id);}
    QMap<qint64, mut_traffic_light*>& getMutTraffL(){return mutTl;}
    mut_traffic_light* getMutTraffLById(qint64 id){return mutTl.value(id);}
    float* getTrafficBreakProbabById(qint64 id){return ps.value(id);}
    void setTrafficBreakProbabById(qint64 id, qreal value){ps.insert(id, new float(value)); rdItems.value(id)->setP(value);}
    QMap<qint64, mut_road*>& getMutRds(){return mutRds;}
    QMap<qint64, float*>& getSpeedRds(){return speedRds;}
    float* getSpeedRdById(qint64 id){return speedRds.value(id);}
    QMap<qint64, float*>& getTrafficBreakProbabs(){return ps;}
    qint64* getTraffLById(int id){return traffLs.value(id);}
    qint64 getTraffLByRoadId(int id);
    QMap<qint64, bool*>& getCongRds(){return congRds;}
    bool* getCongRdById(int id = 0){return congRds.value(id);}
    qint64 timeBetweenPhases(qint64 p1, qint64 p2);
    qint64 completeTime(qint64 p1, qint64 p2);
    qint64 isInRange(qint64 value, qint64 lower, qint64 upper);
    qint64 getPhaseByTime(qint64 value);

    QTimer* getTimer(){return timer;}
    void startTimer(qint64 time = 1000){timer->start(time);}
    void stopTimer(){timer->stop();}
    qint64 getTime(){ return secTime;}

    void setPhase(qint64 value){phase = value;}
    qint64 getPhase(){return phase;}
    qint64 nextPhase(qint64 current);
    qint64 prevPhase(qint64 current);
    QList<qint64> getPhases(){return phases;}
    QList<qint64> getPhasesBetween(qint64 p1, qint64 p2);

    void setGreenTraffL(int id);
    void setYellowTraffL(int id);
    void setRedTraffL(int id);

    void incrDumpCount(){++dump_count;}
    qint64 getDumpCount(){return dump_count;}

    Database* getDatabase(){return db;}

    QMap<qint64, traffic_light*> getTraffLsByRd2TraffL(QMap<qint64, QList<traffic_light>*> values);
    QList<road> getUnallocRoads();

    QMap<qint64, qint64*>& getTimerTls(){return timerTls;}

signals:
    void mapInfo(QString msg, qint64 styleType = INFO);

public slots:
    void updateSecTime();
    void updateTrafficLights();
    void congestionIndicator();
    void preemptTraffL(qint64 id=0);

private:
    qint64 mapType;
    Database *db = nullptr;

    junction junction;
    road single;

    QList<road> upstreamRds;
    QList<qint64> upstreamRdAngles;
    QList<road> downstreamRds;
    QList<qint64> downstreamRdAngles;
    QMultiMap<qint64, road> trfflToRds;

    JunctionGraphicsItem *mainJunction = nullptr;
    RoadGraphicsItem *mainRoad = nullptr;
    QMap<qint64, RoadGraphicsItem*> rdItems;
    QMap<qint64, mut_road*> mutRds;
    QMap<qint64, float*> speedRds;
    QMap<qint64, bool*> congRds;
    QMap<qint64, QList<traffic_light>*> rd2tls;
    QMap<qint64, mut_traffic_light*> preMutTl;
    QMap<qint64, mut_traffic_light*> mutTl;
    QMap<qint64, float*> ps;
    QMap<qint64, qint64*> traffLs;
    QMap<qint64, qint64> congInds;      // congestion indicator i.e 3 sec of congested road

    QPointer<QTimer> timer;

    qint64 secTime = 0;
    qint64 phase;
    qint64 lastCongPhase = -1;
    qint64 currCycle = 0;
    QList<qint64> phases;
    qint64 dump_count = 0;
    bool isTraffLPreempted = false;

    // analytics
    QMap<qint64, QList<traffic_light>*> rd2analy_tls;
    QMap<qint64, qint64*> timerTls;

};

#endif // MAPMODEL_H
