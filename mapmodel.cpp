#include "mapmodel.h"

MapModel::MapModel(qint64 mapType, Database *db, QObject *parent):
    QObject (parent),
    mapType(mapType),
    db(db)
{
    rdItems = QMap<qint64, RoadGraphicsItem*>();
}

void MapModel::build(struct junction j, road r)
{
    rdItems.clear();
    mutRds.clear();
    rd2tls.clear();
    preMutTl.clear();
    mutTl.clear();
    ps.clear();
    speedRds.clear();
    congRds.clear();
    junction = j;
    mainJunction = new JunctionGraphicsItem(j);
    if(mapType == ROAD_MAP_TYPE){
        ROAD_FLOW rf;
        single = r;
        QList<traffic_light> singleTl = db->trafficLightByJunctionIdNRoadId(j.junction_id, r.road_id);
        for(traffic_light i : singleTl){
            trfflToRds.insert(i.traffic_light_id, r);
        }
//        tls.insert(singleTl.traffic_light_id, new traffic_light(singleTl));
//        analy_tls.insert(singleTl.traffic_light_id, new traffic_light(singleTl.traffic_light_id, j.junction_id));
        mutTl.insert(singleTl.first().traffic_light_id, new mut_traffic_light(singleTl.first().traffic_light_id));
        if(db->stringToIds(j.up_road_ids).contains(single.road_id)){
            upstreamRds.insert(single.road_id, single);
            rf = INCOMING;
        }else{
            downstreamRds.insert(single.road_id, single);
            rf = OUTGOING;
        }
        mutRds.insert(single.road_id, new mut_road(single.road_id));
        speedRds.insert(single.road_id, new float(0));
        congRds.insert(single.road_id, new bool(false));
        congInds.insert(single.road_id, 0);
        ps.insert(single.road_id, new float(0));
        mainRoad = new RoadGraphicsItem(single, mutRds.value(single.road_id), 10, rf, ps.value(single.road_id), congRds.value(single.road_id));

        QList<traffic_light> tl = db->trafficLightByJunctionIdNRoadId(junction.junction_id, single.road_id);
        QList<traffic_light>* tlPtr = new QList<traffic_light>();
        for(traffic_light eachtl : tl){
            tlPtr->append(traffic_light(eachtl));
        }
        rd2tls.insert(r.road_id, tlPtr);
        rd2analy_tls.insert(r.road_id, new QList<traffic_light> (*tlPtr));

        rdItems.insert(single.road_id, mainRoad);
    }else if(mapType == JUNCTION_MAP_TYPE){
        // traffic lights
        for(traffic_light i : db->trafficLightsByJunctionId(j.junction_id)){
            for(int j : db->stringToIds(i.road_ids)){
                trfflToRds.insertMulti(i.traffic_light_id, db->roadById(j));
            }
            mutTl.insert(i.traffic_light_id, new mut_traffic_light(i.traffic_light_id));
        }
        // upstream roads
        for(int i : db->stringToIds(j.up_road_ids)){
            road r = db->roadById(i);
            if(!trfflToRds.values().contains(r))
                trfflToRds.insertMulti(-1, r);
            upstreamRds.append(r);
        }
        upstreamRdAngles = db->stringToAngles(j.up_road_angles);
        //
        for(int i = 0; i < upstreamRds.size(); i++){
            road r = upstreamRds.at(i);

            QList<traffic_light> tl = db->trafficLightByJunctionIdNRoadId(junction.junction_id, r.road_id);
            QList<traffic_light>* tlPtr = new QList<traffic_light>();
            for(traffic_light eachtl : tl){
                tlPtr->append(traffic_light(eachtl));
            }
            rd2tls.insert(r.road_id, tlPtr);
            rd2analy_tls.insert(r.road_id, new QList<traffic_light> (*tlPtr));

            int angle = upstreamRdAngles.at(i);
            mutRds.insert(r.road_id, new mut_road(r.road_id));
            speedRds.insert(r.road_id, new float(0));
            congRds.insert(r.road_id, new bool(false));
            congInds.insert(r.road_id, 0);
            ps.insert(r.road_id, new float(0));
            rdItems.insert(r.road_id, new RoadGraphicsItem(r, mutRds.value(r.road_id), angle, INCOMING, ps.value(r.road_id), congRds.value(r.road_id)));
        }
        //
        // downstream roads
        for(int i : db->stringToIds(j.down_road_ids)){
            road r = db->roadById(i);
            if(!trfflToRds.values().contains(r))
                trfflToRds.insertMulti(-1, r);
            downstreamRds.append(r);
        }
        downstreamRdAngles = db->stringToIds(j.down_road_angles);
        //
        for(int i = 0; i < downstreamRds.size(); i++){
            road r = downstreamRds.at(i);

            QList<traffic_light> tl = db->trafficLightByJunctionIdNRoadId(junction.junction_id, r.road_id);
            QList<traffic_light>* tlPtr = new QList<traffic_light>();
            for(traffic_light eachtl : tl){
                tlPtr->append(traffic_light(eachtl));
            }
            rd2tls.insert(r.road_id, tlPtr);
            rd2analy_tls.insert(r.road_id, new QList<traffic_light> (*tlPtr));

            int angle = downstreamRdAngles.at(i);
            mutRds.insert(r.road_id, new mut_road(r.road_id));
            speedRds.insert(r.road_id, new float(0));
            congRds.insert(r.road_id, new bool(false));
            congInds.insert(r.road_id, 0);
            ps.insert(r.road_id, new float(0));
            rdItems.insert(r.road_id, new RoadGraphicsItem(r, mutRds.value(r.road_id), angle, OUTGOING, ps.value(r.road_id), congRds.value(r.road_id)));
        }
        //
    }else{
        // soon
    }
    phases = trfflToRds.uniqueKeys();
    for(qint64 i : phases){
        qDebug() << "PHASE: " << i;
    }
    phase = phases.first();
    qDebug() << "FIRST PHASE: " << phase;
    buildTrafficLights();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateSecTime()));
    connect(timer, SIGNAL(timeout()), this, SLOT(congestionIndicator()));
    emit mapInfo(QString(" Building Map of JUNCTION: %1 with %2 no. of roads.").arg(junction.junction_id).arg(getAllRds().size()));

//    // displaying roads by traffic light
//    for(qint64 id : trfflToRds.uniqueKeys()){
//        qDebug() << "TL ID: " << id;
//        for(road i : trfflToRds.values(id)){
//            qDebug() << repr_road(i);
//        }
//    }
}

void MapModel::buildTrafficLights()
{
    for(qint64 i : getAllRds().keys()){
        traffLs.insert(i, new qint64(NO_SIGNAL));
    }

    if(trfflToRds.values(phase).size() > 0){
        for(road i : trfflToRds.values(phase)){
            qDebug() << "GREENING ROAD ID: " << i.road_id;
            traffLs.insert(i.road_id, new qint64(GREEN_SIGNAL));
        }
        qDebug() << "___________________________";
    }

    for(qint64 i : traffLs.keys()){
        rdItems.value(i)->setTraffL(traffLs.value(i));
    }

    for(mut_traffic_light* i : mutTl.values()){
        mut_traffic_light mut_db = db->onPumpTrafficLight(i->traffic_light_id);
        i->green_timer = mut_db.green_timer;
        qDebug() << "TL ID: " << i->traffic_light_id << " TIMER: " << i->green_timer;
        preMutTl.insert(i->traffic_light_id, new mut_traffic_light(mut_db));
    }
}

JunctionGraphicsItem *MapModel::getJunctionGraphicsItem()
{
    if(mainJunction != nullptr)
        return mainJunction;
    struct junction j;
    j.junction_id = -1;
    j.name = "UNDEFINED_NAME";
    j.area = 10;
    return new JunctionGraphicsItem(j);
}

RoadGraphicsItem *MapModel::getRoadGraphicsItem()
{
    if(mainRoad != nullptr)
        return mainRoad;
    struct road r;
    r.road_id = -1;
    r.name = "UNDEFINED_NAME";
    r.length = 10;
    r.lanes = 2;
    return new RoadGraphicsItem(r);
}

QMap<qint64, RoadGraphicsItem*> MapModel::getRoadsGraphicsItem()
{
    return rdItems;
}

MapModel::~MapModel()
{
//    qDeleteAll(rdItems);
//    qDeleteAll(mutRds);
//    qDeleteAll(ps);
//    if(db != nullptr)
//        delete db;
//    if(mainJunction != nullptr)
//        delete mainJunction;
//    if(mainRoad != nullptr)
//        delete mainRoad;
}

QMap<qint64, road> MapModel::getAllRds()
{
    QMap<qint64, road> out;
    for(road i : getUpstreamRds())
        out.insert(i.road_id, i);
    for(road i : getDownstreamRds())
        out.insert(i.road_id, i);
    return out;
}

qint64 MapModel::getTraffLByRoadId(int id)
{
    for(qint64 traffL : trfflToRds.keys()){
        for(road i : trfflToRds.values(traffL)){
            if(i.road_id == id){
                // qDebug() << "TL OF ROAD: " << i.road_id << " IS " << traffL;
                return traffL;
            }
        }
    }
    return -1;
}

qint64 MapModel::timeBetweenPhases(qint64 p1, qint64 p2)
{
    qint64 out = 0;

    if(p1 < p2){
        for(int i = phases.indexOf(p1)+1; i < phases.indexOf(p2); i++){
            out += mutTl.value(phases.at(i))->green_timer;
        }
    }else if(p1 > p2){
        for(int i = phases.indexOf(p1)+1; i < phases.size(); i++){
            out += mutTl.value(phases.at(i))->green_timer;
        }
        for(int i = 0; i < phases.indexOf(p2); i++){
            out += mutTl.value(phases.at(i))->green_timer;
        }
    }

    // qDebug() << "TIME DIFFERENCE BETWEEN " << db->onPumpTrafficLight(p1).traffic_light_id << " AND " << db->onPumpTrafficLight(p2).traffic_light_id << " IS " << out;
    return out;
}

qint64 MapModel::completeTime(qint64 p1, qint64 p2)
{
    qint64 out = 0;

    if(p1 <= p2){
        for(int i = phases.indexOf(p1); i <= phases.indexOf(p2); i++){
            out += mutTl.value(phases.at(i))->green_timer;
        }
    }else{
        for(int i = phases.indexOf(p2); i <= phases.indexOf(p1); i++){
            out += mutTl.value(phases.at(i))->green_timer;
        }
    }
    return out;
}

qint64 MapModel::isInRange(qint64 value, qint64 lower, qint64 upper)
{
    if(value >= lower && value < upper)
        return true;
    return false;
}

qint64 MapModel::getPhaseByTime(qint64 value)
{
    for(qint64 i : phases){
        mut_traffic_light currTl = *mutTl.value(i);
        qint64 timeTill = completeTime(phases.first(), i);
        if(isInRange(value, timeTill-currTl.green_timer, timeTill)){
            return i;
        }
    }
    return -1;
}

qint64 MapModel::nextPhase(qint64 current)
{
    if (current == phases.last())
        return phases.last();
    return phases.at(phases.indexOf(current)+1);
}

qint64 MapModel::prevPhase(qint64 current)
{
    if(current == phases.first())
        return 0;
    return phases.at(phases.indexOf(current)-1);
}

QList<qint64> MapModel::getPhasesBetween(qint64 p1, qint64 p2)
{
    QList<qint64> tls;

    return tls;
}

void MapModel::setGreenTraffL(int id)
{
    for(road i : trfflToRds.values(id)){
        traffLs.insert(i.road_id, new qint64(GREEN_SIGNAL));
    }
}

void MapModel::setYellowTraffL(int id)
{
    for(road i : trfflToRds.values(id)){
        traffLs.insert(i.road_id, new qint64(YELLOW_SIGNAL));
    }
}

void MapModel::setRedTraffL(int id)
{
    for(road i : trfflToRds.values(id)){
        if(!trfflToRds.values(phase).contains(i)){
            traffLs.insert(i.road_id, new qint64(RED_SIGNAL));
        }
    }
}

QMap<qint64, traffic_light*> MapModel::getTraffLsByRd2TraffL(QMap<qint64, QList<traffic_light> *> values)
{
    QMap<qint64, traffic_light*> out;
    QMap<qint64, QList<QString>*> rdIds;
    for(mut_traffic_light *i : mutTl.values()){
        if(i->traffic_light_id != -1){
            out.insert(i->traffic_light_id, new traffic_light(i->traffic_light_id, junction.junction_id));
            rdIds.insert(i->traffic_light_id, new QList<QString>());
        }
    }
    for(qint64 road_id : values.keys()){
        for(traffic_light i : *values.value(road_id)){
            rdIds.value(i.traffic_light_id)->append(QString("%1").arg(road_id));
            qDebug() << "Adding " << i.traffic_light_id << " to " << road_id << " SIZEOF rdIds: " << rdIds.value(i.traffic_light_id)->size();
        }
    }
    for(traffic_light* eachTl : out.values()){
        QStringList eachRdIds = QStringList(*rdIds.value(eachTl->traffic_light_id));
        QString road_ids = eachRdIds.join(",");
        eachTl->road_ids = road_ids;
        qDebug() << "TL ID: " << eachTl->traffic_light_id << " ROAD IDS: " << eachTl->road_ids;
    }

    return out;
}

QList<road> MapModel::getUnallocRoads()
{
    return trfflToRds.values(-1);
}

void MapModel::updateSecTime()
{
    secTime += 1;
    // qDebug() << "P: " << rdItems.first()->getP();
}

void MapModel::updateTrafficLights()
{
    mut_traffic_light currTrffL = *mutTl.value(phase);
    currCycle = secTime % completeTime(phases.first(), phases.last());
    qint64 oldPhase = phase;
    phase = getPhaseByTime(currCycle);
    // use count
    if(oldPhase != phase){
        getMutTraffLById(phase)->use_count += 1;
    }
    for(qint64 id: trfflToRds.keys()){
        if(id == currTrffL.traffic_light_id){
            setGreenTraffL(id);
        }else{
            setRedTraffL(id);
        }
    }
    for(qint64 i : traffLs.keys()){
        rdItems.value(i)->setTraffL(traffLs.value(i));
    }
}

void MapModel::congestionIndicator()
{
    if(phase == nextPhase(lastCongPhase))
        isTraffLPreempted = false;
    for(qint64 id : congRds.keys()){
        if(*congRds.value(id) == true){
            congInds.insert(id, congInds.value(id)+1);
        }else{
            congInds.insert(id, 0);
        }
    }
    for(qint64 id : congInds.keys()){
        if(congInds.value(id) >= 2){
            if(!isTraffLPreempted){
                preemptTraffL(id);
                break;
            }
        }
    }
}

void MapModel::preemptTraffL(qint64 id)
{
    qint64 selectedTraffL = getTraffLByRoadId(id);
    // qDebug() << "Current TL ID: " << phase << " TO;BE ID: " << selectedTraffL;
    if(selectedTraffL != phase){
        emit mapInfo(QString("Congestion at Road: %1. Preempting current phase").arg(id), WARNING);
//        qDebug() << "Original time: " << secTime;
        isTraffLPreempted = true;
        if(lastCongPhase != selectedTraffL){
            mutRds.value(id)->congestion_count += 1;
            emit mapInfo(QString("Road: %1 Congsetion Count: %2").arg(id).arg(mutRds.value(id)->congestion_count), ERROR);
        }
        lastCongPhase = selectedTraffL;

        mut_traffic_light currTl = db->onPumpTrafficLight(phase);
        qint64 currTlTime = currCycle;
//        qDebug() << "PRE: currTlTime: " << currTlTime;
        if(phase != phases.first()){
            currTlTime -= completeTime(phases.first(), phase);
        }
//        qDebug() << "currTlTime: " << currTlTime;
        qint64 step = currTl.green_timer - currTl.green_timer%currTlTime;
        secTime += step;
//        qDebug() << "GREEN TIME: " << currTl.green_timer << " step: " << step;
//        qDebug() << "seCTime: " << secTime;

        if(selectedTraffL != -1){
            secTime += timeBetweenPhases(phase, selectedTraffL);
        }else{
            qDebug() << "Invalid Traffic Light ID";
        }

//        qDebug() << "Updated time: " << secTime;
    }else{
        emit mapInfo(QString("Congestion at Road: %1. No change").arg(id), VERBOSE);
    }
}
