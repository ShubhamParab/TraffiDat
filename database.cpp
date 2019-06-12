#include "database.h"
using namespace TraffiDat;

static int countDb = 0;
Database::Database(QString filename, QObject *parent)
    :QObject(parent),
      filename(filename)
{
    // setup
    db = QSqlDatabase::addDatabase("QSQLITE", tr("CONNECT_%1").arg(countDb++));
    db.setDatabaseName(filename);
    q = new QSqlQuery(db);
    connect(this, SIGNAL(dbInfo(QString,qint64)), parent, SLOT(addLog(QString,qint64)));
    // opening
    if(!db.open()){
        emit dbInfo(tr("Error: Opening database file: {}").arg(filename));
    }else{
        QStringList tables = db.tables(QSql::TableType::Tables);
        if(!tables.contains("ROAD")){
            if(q->exec("CREATE TABLE ROAD ("
                        "ROAD_ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                        "NAME TEXT,"
                        "LENGTH REAL,"
                        "LANES INTEGER,"
                        "PERC_DAMAGE REAL,"
                        "VIDEO_ADDR TEXT);")){
                emit dbInfo("INFO: CREATE TABLE ROAD SUCCESSFULLY!");
            }else{
                db.rollback();
                emit dbInfo("ERROR: CREATE TABLE ROAD FAILED :<", ERROR);
                emit dbInfo(q->lastError().text());
            }
        }
        if(!tables.contains("MUT_ROAD")){
            if(q->exec("CREATE TABLE MUT_ROAD ("
                       "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "ROAD_ID INTEGER,"
                       "VEHICLE_COUNT INTEGER,"
                       "VEHICLE_SPEED REAL,"
                       "CONGESTION_COUNT INTEGER,"
                       "TIMESTAMP TEXT,"
                       "FOREIGN KEY(ROAD_ID) REFERENCES ROAD(ROAD_ID));")){
                emit dbInfo("INFO: CREATE TABLE MUT_ROAD SUCCESSFULLY!");
            }else{
                db.rollback();
                emit dbInfo("ERROR: CREATE TABLE MUT_ROAD FAILED :<", ERROR);
                emit dbInfo(q->lastError().text());
            }
        }
        if(!tables.contains("MUT_TRAFFIC_LIGHT")){
            if(q->exec("CREATE TABLE MUT_TRAFFIC_LIGHT ("
                       "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "TRAFFIC_LIGHT_ID INTEGER,"
                       "GREEN_TIMER INTEGER,"
                       "USE_COUNT INTEGER,"
                       "TIMESTAMP TEXT,"
                       "FOREIGN KEY(TRAFFIC_LIGHT_ID) REFERENCES TRAFFIC_LIGHT(TRAFFIC_LIGHT_ID));")){
                emit dbInfo("INFO: CREATE TABLE MUT_TRAFFIC_LIGHT SUCCESSFULLY!");
            }else{
                db.rollback();
                emit dbInfo("ERROR: CREATE TABLE MUT_TRAFFIC_LIGHT FAILE :<", ERROR);
                emit dbInfo(q->lastError().text());
            }
        }
        if(!tables.contains("JUNCTION")){
            if(q->exec("CREATE TABLE JUNCTION ("
                       "JUNCTION_ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "NAME TEXT,"
                       "AREA REAL,"
                       "UP_ROAD_IDS TEXT,"
                       "UP_ROAD_ANGLES TEXT,"
                       "DOWN_ROAD_IDS TEXT,"
                       "DOWN_ROAD_ANGLES TEXT);")){
                emit dbInfo("INFO: CREATE TABLE JUNCTION SUCCESSFULLY!");
            }else{
                db.rollback();
                emit dbInfo("ERROR: CREATE TABLE JUNCTION FAILED :<", ERROR);
                emit dbInfo(q->lastError().text());
            }
        }
        if(!tables.contains("TRAFFIC_LIGHT")){
            if(q->exec("CREATE TABLE TRAFFIC_LIGHT ("
                       "TRAFFIC_LIGHT_ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "JUNCTION_ID INTEGER, "
                       "ROAD_IDS TEXT,"
                       "FOREIGN KEY(JUNCTION_ID) REFERENCES JUNCTION(JUNCTION_ID));")){
                emit dbInfo("INFO: CREATE TABLE TRAFFIC_LIGHT SUCCESSFULLY!");
            }else{
                db.rollback();
                emit dbInfo("ERROR: CREATE TABLE TRAFFIC_LIGHT FAILED :<", ERROR);
                emit dbInfo(q->lastError().text());
            }
        }
        mutRoadModel = new QSqlTableModel(this, db);
        mutRoadModel->setTable("MUT_ROAD");
        mutRoadModel->select();

        mutTrafficLightModel = new QSqlTableModel(this, db);
        mutTrafficLightModel->setTable("MUT_TRAFFIC_LIGHT");
        mutTrafficLightModel->select();

        roadModel = new QSqlTableModel(this, db);
        roadModel->setTable("ROAD");
        roadModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        roadModel->select();

        trafficLightModel = new QSqlTableModel(this, db);
        trafficLightModel->setTable("TRAFFIC_LIGHT");
        trafficLightModel->select();

        junctionModel = new QSqlTableModel(this, db);
        junctionModel->setTable("JUNCTION");
        junctionModel->select();
    }
}

void Database::buildMeanValues()
{
    meanMutRds.clear();
//    QMultiMap<qint64, mut_road> allMutRds;
//    for(int i = 0; i < mutRoadModel->rowCount(); i++){
//        mut_road each = sqlToModMutRoad(mutRoadModel->record(i));
//        allMutRds.insert(each.road_id, each);
//    }
//    if(allMutRds.size() > 0){
//        for(qint64 i : allMutRds.keys()){
//            meanMutRds.insert(i, getMeanMutRoad(allMutRds.values(i)));
//        }
//    }else{
        for(road i : getAvailRoads()){
            mut_road each(i.road_id);
            each.vehicle_count = 70;
            each.vehicle_speed = 70;
            meanMutRds.insert(i.road_id, each);
        }
//    }
}

mut_road Database::getMeanMutRoad(QList<mut_road> roads)
{
    mut_road out(roads.first().road_id);
    out.vehicle_count = 1;
    out.vehicle_speed = 1;
    out.timestamp = "MEAN TIME";
    // speed average
    if(roads.size() > 0){
        for(mut_road i : roads){
            out.vehicle_speed += i.vehicle_speed;
        }
        out.vehicle_speed /= 2*roads.size();
    }
    // count average
    road r = roadById(out.road_id);
    qint64 totalArea = r.length*r.lanes;
    qint64 damageArea = (r.perc_damage/100)*totalArea;
    qint64 availArea = totalArea-damageArea;
    out.vehicle_count = availArea/(2*VEHICLE_LENGTH);

    qDebug() << "ROAD ID: " << out.road_id << "Mean Vehicle count: " << out.vehicle_count << " Sample size: " << roads.size();
    return out;
}

bool Database::addRoad(struct road r)
{
    if(q->prepare("INSERT INTO ROAD (NAME, LENGTH, LANES, PERC_DAMAGE, VIDEO_ADDR)"
                  "VALUES (:name, :length, :lanes, :perc_damage, :video_addr);")){
        q->bindValue(":name", r.name);
        q->bindValue(":length", QString::number(r.length));
        q->bindValue(":lanes", r.lanes);
        q->bindValue(":perc_damage", QString::number(r.perc_damage));
        q->bindValue(":video_addr", r.video_addr);
        if(q->exec()){
            roadModel->select();
            emit dbChanged();
            emit dbInfo("INFO: Adding new road");
            return true;
        }else{
            db.rollback();
            emit dbInfo("ERROR: Adding new road", ERROR);
            return false;
        }
    }else{
        db.rollback();
        emit dbInfo("ERROR: Preparing addRoad query", ERROR);
    }
    return false;
}

bool Database::addTrafficLight(struct traffic_light tl)
{
    if(q->prepare("INSERT INTO TRAFFIC_LIGHT (JUNCTION_ID, ROAD_IDS)"
                  "VALUES (:junction_id, :road_ids);")){
        q->bindValue(":junction_id", tl.junction_id);
        q->bindValue(":road_ids", tl.road_ids);
        if(q->exec()){
            trafficLightModel->select();
            emit dbChanged();
            emit dbInfo("INFO: Adding new traffic light");
            return true;
        }else{
            db.rollback();
            emit dbInfo("ERROR: Adding new traffic light", ERROR);
            return false;
        }
    }else{
        db.rollback();
        emit dbInfo("ERROR: Adding new traffic light", ERROR);
    }
    return false;
}

bool Database::removeTrafficLight(traffic_light tl)
{
    if(q->prepare("DELETE FROM TRAFFIC_LIGHT WHERE TRAFFIC_LIGHT_ID = :traffic_light_id;")){
        q->bindValue(":traffic_light_id", tl.traffic_light_id);
        if(q->exec()){
            trafficLightModel->select();
            emit dbChanged();
            emit dbInfo("INFO: Removing traffic light");

            removeMutTl(mut_traffic_light(tl.traffic_light_id));

            return true;
        }else{
            db.rollback();
            emit dbInfo("ERROR: Removing traffic light", ERROR);
            return false;
        }
    }else{
        db.rollback();
        emit dbInfo("ERROR: Removing traffic light", ERROR);
    }
    return false;
}

bool Database::removeTrafficLightByJunctionId(qint64 id)
{
    if(q->prepare("DELETE FROM TRAFFIC_LIGHT WHERE JUNCTION_ID = :junction_id;")){
        q->bindValue(":junction_id", id);
        if(q->exec()){
            trafficLightModel->select();
            emit dbChanged();
            emit dbInfo("INFO: Removing traffic light");

            return true;
        }else{
            db.rollback();
            emit dbInfo("ERROR: Removing traffic light", ERROR);
            return false;
        }
    }else{
        db.rollback();
        emit dbInfo("ERROR: Removing traffic light", ERROR);
    }
    return false;
}

bool Database::replaceTrafficLight(traffic_light tl)
{
    if(getAvailTraffL().contains(tl)){
        updateTrafficLight(tl);
        return true;
    }else{
        return addTrafficLight(tl);
    }
}

bool Database::addJunction(struct junction j)
{
    if(q->prepare("INSERT INTO JUNCTION (NAME, AREA, UP_ROAD_IDS, UP_ROAD_ANGLES, DOWN_ROAD_IDS, DOWN_ROAD_ANGLES)"
                  "VALUES (:name, :area, :up_road_ids, :up_road_angles, :down_road_ids, :down_road_angles);")){
        q->bindValue(":name", j.name);
        q->bindValue(":area", QString::number(j.area));
        q->bindValue(":up_road_ids", j.up_road_ids);
        q->bindValue(":up_road_angles", j.up_road_angles);
        q->bindValue(":down_road_ids", j.down_road_ids);
        q->bindValue(":down_road_angles", j.down_road_angles);
        if(q->exec()){
            junctionModel->select();
            emit dbChanged();
            emit dbInfo("INFO: Adding new junction");
            emit dbInfo(repr_junction(j));
            return true;
        }else{
            db.rollback();
            emit dbInfo("ERROR: Adding new junction", ERROR);
            return false;
        }
    }else{
        db.rollback();
        emit dbInfo("ERROR: Adding new junction", ERROR);
    }
    return false;
}

bool Database::removeJunction(junction j)
{
    if(q->prepare("DELETE FROM JUNCTION WHERE JUNCTION_ID = :junction_id;")){
        q->bindValue(":junction_id", j.junction_id);
        if(q->exec()){
            junctionModel->select();
            emit dbChanged();
            emit dbInfo("INFO: Removing junction");

            return true;
        }else{
            db.rollback();
            emit dbInfo("ERROR: Removing junction", ERROR);
            return false;
        }
    }else{
        db.rollback();
        emit dbInfo("ERROR: Removing junction", ERROR);
    }
    return false;
}

void Database::updateRoad(road r)
{
    if(q->prepare("UPDATE ROAD SET NAME = :name, LENGTH = :length, LANES = :lanes, PERC_DAMAGE = :perc_damage, VIDEO_ADDR = :video_addr WHERE ROAD_ID = :road_id;")){
        q->bindValue(":road_id", r.road_id);
        q->bindValue(":name", r.name);
        q->bindValue(":length", r.length);
        q->bindValue(":lanes", r.lanes);
        q->bindValue(":perc_damage", r.perc_damage);
        q->bindValue(":video_addr", r.video_addr);
        if(q->exec()){
            roadModel->select();
            emit dbChanged();
            emit dbInfo("INFO: Updating exisiting road");
        }else{
            db.rollback();
            emit dbInfo("ERROR: Updating existing road", ERROR);
        }
    }else{
        db.rollback();
        emit dbInfo("ERROR: Preparing updateRoad query", ERROR);
    }
}

void Database::updateTrafficLight(traffic_light tl)
{
    if(q->prepare("UPDATE TRAFFIC_LIGHT SET JUNCTION_ID = :junction_id, ROAD_IDS = :road_ids WHERE TRAFFIC_LIGHT_ID = :traffic_light_id;")){
        q->bindValue(":traffic_light_id", tl.traffic_light_id);
        q->bindValue(":junction_id", tl.junction_id);
        q->bindValue(":road_ids", tl.road_ids);
        if(q->exec()){
            trafficLightModel->select();
            emit dbChanged();
            emit dbInfo("INFO: Updating existing traffic_light");
        }else{
            db.rollback();
            emit dbInfo("ERROR: Updating existing traffic_light", ERROR);
        }
    }else{
        db.rollback();
        emit dbInfo("ERROR: Preparing updateRoad query", ERROR);
    }
}

void Database::updateJunction(junction j)
{
    if(q->prepare("UPDATE JUNCTION SET NAME = :name, AREA = :area, UP_ROAD_IDS = :up_road_ids, UP_ROAD_ANGLES = :up_road_angles, DOWN_ROAD_IDS = :down_road_ids, DOWN_ROAD_ANGLES = :down_road_angles WHERE JUNCTION_ID = :junction_id;")){
        q->bindValue(":junction_id", j.junction_id);
        q->bindValue(":name", j.name);
        q->bindValue(":area", j.area);
        q->bindValue(":up_road_ids", j.up_road_ids);
        q->bindValue("up_road_angles", j.up_road_angles);
        q->bindValue("down_road_ids", j.down_road_ids);
        q->bindValue("down_road_angles", j.down_road_angles);
        if(q->exec()){
            junctionModel->select();
            emit dbChanged();
            emit dbInfo("INFO: Updating existing traffic_light");
        }else{
            db.rollback();
            emit dbInfo("ERROR: Updating existing traffic_light", ERROR);
        }
    }else{
        db.rollback();
        emit dbInfo("ERROR: Preparing updateRoad query", ERROR);
    }
}

QList<road> Database::getAvailRoads()
{
    QList<road> out = QList<road>();
    for(int i = 0; i < roadModel->rowCount(); i++){
        out.append(sqlToModRoad(roadModel->record(i)));
    }

    return out;
}

road Database::roadById(qint64 id)
{
    for(road i : getAvailRoads()){
        if(i.road_id == id)
            return i;
    }
    return road();
}

QList<road> Database::roadsByJunction(junction j)
{
    QList<road> out;
    for(qint64 i : stringToIds(j.up_road_ids)){
        out.append(roadById(i));
    }
    for(qint64 i : stringToIds(j.down_road_ids)){
        out.append(roadById(i));
    }
    return out;
}

QList<junction> Database::getAvailJuncts()
{
    QList<junction> out = QList<junction>();
    for(qint64 i = 0; i < junctionModel->rowCount(); i++){
        out.append(sqlToModJunction(junctionModel->record(i)));
    }

    return out;
}

junction Database::junctionById(qint64 id)
{
    for(junction j : getAvailJuncts()){
        if(j.junction_id == id)
            return j;
    }
    return junction(-1);
}

QList<traffic_light> Database::getAvailTraffL()
{
    QList<traffic_light> out = QList<traffic_light>();
    for(int i = 0; i < trafficLightModel->rowCount(); i++){
        out.append(sqlToModTrafficL(trafficLightModel->record(i)));
    }

    return out;
}

QList<qint64> Database::getUnallocTraffL(qint64 value)
{
    QList<qint64> out;
    qint64 i = 1;
    while(out.size() != value){
        if(!getAvailTraffL().contains(traffic_light(i)))
            out.append(i);
        i++;
    }
    qDebug() << "Unalloc traffLs: " << out.size();
    return out;
}

traffic_light Database::trafficLightById(qint64 id)
{
    for(traffic_light tl : getAvailTraffL()){
        if(tl.traffic_light_id == id)
            return tl;
    }
    return traffic_light();
}

QList<traffic_light> Database::trafficLightsByJunctionId(qint64 id)
{
    QList<traffic_light> tls;
    for(traffic_light i : getAvailTraffL()){
        if(i.junction_id == id)
            tls.append(i);
    }
    return tls;
}

QList<traffic_light> Database::trafficLightByJunctionIdNRoadId(qint64 junctId, qint64 rdId)
{
    QList<traffic_light> out;
    for(traffic_light tl : trafficLightsByJunctionId(junctId)){
        for(int id : stringToIds(tl.road_ids)){
            if(rdId == id)
                out.append(tl);
        }
    }
    // incase of empty traffic_light obj
    if(out.size() == 0){
        traffic_light freeflow(-1, junctId);
        out.append(freeflow);
    }
    return out;
}

QMultiMap<qint64, mut_road> Database::getAllMutRoads()
{
    QMultiMap<qint64, mut_road> out;
    for(int i = 0; i < mutRoadModel->rowCount(); i++){
        mut_road each = sqlToModMutRoad(mutRoadModel->record(i));
        out.insertMulti(each.road_id, each);
    }
    return out;
}

QList<mut_road> Database::getAllMutRoadsById(qint64 id, qint64 sampleCount)
{
    QList<mut_road> all = getAllMutRoads().values(id);
    if(sampleCount == -1)
        return all;
    QList<mut_road> out;
    for(int i = 0; i < sampleCount; i++){
        if(!all.isEmpty()){
            out.append(all.takeFirst());
        }
    }
    return out;
}

bool Database::removeMutRoad(mut_road mr)
{
    if(q->prepare("DELETE FROM MUT_ROAD WHERE TRAFFIC_LIGHT_ID = :road_id;")){
        q->bindValue(":road_id", mr.road_id);
        if(q->exec()){
            mutRoadModel->select();
            emit dbChanged();
            emit dbInfo(QString("INFO: Removing mut road:%1").arg(mr.road_id));
            return true;
        }else{
            db.rollback();
            emit dbInfo(QString("ERROR: Removing mut road:%1").arg(mr.road_id), ERROR);
            return false;
        }
    }else{
        db.rollback();
        emit dbInfo(QString("ERROR: Removing mut road:%1").arg(mr.road_id), ERROR);
    }
    return false;
}

QMultiMap<qint64, QPair<qint64, mut_traffic_light>> Database::getAllMutTraffLs()
{
    QMultiMap<qint64, QPair<qint64, mut_traffic_light>> out;
    if(q->exec("SELECT * FROM MUT_TRAFFIC_LIGHT;")){
        while(q->next()){
            mut_traffic_light each;
            each.id = q->value("ID").toInt();
            each.timestamp = q->value("TIMESTAMP").toString();
            each.use_count = q->value("USE_COUNT").toInt();
            each.green_timer = q->value("GREEN_TIMER").toInt();
            each.traffic_light_id = q->value("TRAFFIC_LIGHT_ID").toInt();
            out.insertMulti(each.traffic_light_id, QPair<qint64, mut_traffic_light>(each.id, each));
        }
    }
    qDebug() << "ALL TL SIZE: " << out.size();
    return out;
}

QList<mut_traffic_light> Database::getAllMutTraffLsById(qint64 id, qint64 sampleCount)
{
    QList<mut_traffic_light> out;
    QMap<qint64, mut_traffic_light> allPairs;
    for(QPair<qint64, mut_traffic_light> i : getAllMutTraffLs().values(id)){
        allPairs.insert(i.first, i.second);
    }
    if(sampleCount == -1)
        return allPairs.values();

    if(allPairs.size() > 0 && allPairs.size() >= sampleCount){
        for(int i = 0; i < sampleCount; i++){
            out.append(allPairs.values().at(allPairs.size()-i-1));
        }
    }
    qDebug() << "allPairs size: " << allPairs.size();
    return out;
}

bool Database::removeMutTl(mut_traffic_light mtl)
{
    if(q->prepare("DELETE FROM MUT_TRAFFIC_LIGHT WHERE TRAFFIC_LIGHT_ID = :traffic_light_id;")){
        q->bindValue(":traffic_light_id", mtl.traffic_light_id);
        if(q->exec()){
            trafficLightModel->select();
            emit dbChanged();
            emit dbInfo("INFO: Removing traffic light");
            return true;
        }else{
            db.rollback();
            emit dbInfo("ERROR: Removing traffic light", ERROR);
            return false;
        }
    }else{
        db.rollback();
        emit dbInfo("ERROR: Removing traffic light", ERROR);
    }
    return false;
}

void Database::saveAll()
{
    mutRoadModel->submitAll();
    mutTrafficLightModel->submitAll();
    roadModel->submitAll();
    trafficLightModel->submitAll();
    junctionModel->submitAll();
}

Database::~Database()
{
    db.close();
//    if(q != nullptr)
//        delete q;
//    if(mutRoadModel != nullptr)
//        delete mutRoadModel;
//    if(mutTrafficLightModel != nullptr)
//        delete mutTrafficLightModel;
//    if(roadModel != nullptr)
//        delete roadModel;
//    if(trafficLightModel != nullptr)
//        delete trafficLightModel;
//    if(junctionModel != nullptr)
//        delete junctionModel;

}

mut_road Database::sqlToModMutRoad(QSqlRecord r)
{
    mut_road out(-1);
    out.id = r.value("ID").toInt();
    out.road_id = r.value("ROAD_ID").toInt();
    out.vehicle_count = r.value("VEHICLE_COUNT").toInt();
    out.vehicle_speed = r.value("VEHICLE_SPEED").toFloat();
    out.congestion_count = r.value("CONGESTION_COUNT").toInt();
    out.timestamp = r.value("TIMESTAMP").toString();

    return out;
}

QSqlRecord Database::modToSqlMutRoad(mut_road r)
{
    QSqlRecord out;
    out.setValue("ID", r.id);
    out.setValue("ROAD_ID", r.road_id);
    out.setValue("VEHICLE_COUNT", r.vehicle_count);
    out.setValue("VEHICLE_SPEED", QString::number(r.vehicle_speed));
    out.setValue("CONGESTION_COUNT", r.congestion_count);
    out.setValue("TIMESTAMP", r.timestamp);

    return out;
}

QSqlRecord Database::modToSqlRoad(road r)
{
    QSqlRecord out;
    out.setValue("ROAD_ID", r.road_id);
    out.setValue("NAME", r.name);
    out.setValue("LENGTH", r.length);
    out.setValue("LANES", r.lanes);
    out.setValue("PERC_DAMAGE", r.perc_damage);
    out.setValue("VIDEO_ADDR", r.video_addr);

    return out;
}

mut_traffic_light Database::sqlToModMutTraffL(QSqlRecord r)
{
    mut_traffic_light out(-1);
    out.id = r.value("ID").toInt();
    out.traffic_light_id = r.value("TRAFFIC_LIGHT_ID").toInt();
    out.green_timer = r.value("GREEN_TIMER").toInt();
    out.use_count = r.value("USE_COUNT").toInt();
    out.timestamp = r.value("TIMESTAMP").toString();

    return out;
}

QSqlRecord Database::modToSqlMutTraffL(mut_traffic_light r)
{
    QSqlRecord out;
    out.setValue("ID", r.id);
    out.setValue("TRAFFIC_LIGHT_ID", r.traffic_light_id);
    out.setValue("GREEN_TIMER", r.green_timer);
    out.setValue("USE_COUNT", r.use_count);
    out.setValue("TIMESTAMP", r.timestamp);

    return out;
}

road Database::sqlToModRoad(QSqlRecord r)
{
    road out;
    out.road_id = r.value("ROAD_ID").toInt();
    out.name = r.value("NAME").toString();
    out.length = r.value("LENGTH").toFloat();
    out.lanes = r.value("LANES").toInt();
    out.perc_damage = r.value("PERC_DAMAGE").toFloat();
    out.video_addr = r.value("VIDEO_ADDR").toString();

    return out;
}

junction Database::sqlToModJunction(QSqlRecord r)
{
    junction out;
    out.junction_id = r.value("JUNCTION_ID").toInt();
    out.name = r.value("NAME").toString();
    out.area = r.value("AREA").toFloat();
    out.up_road_ids = r.value("UP_ROAD_IDS").toString();
    out.up_road_angles = r.value("UP_ROAD_ANGLES").toString();
    out.down_road_ids = r.value("DOWN_ROAD_IDS").toString();
    out.down_road_angles = r.value("DOWN_ROAD_ANGLES").toString();

    return out;
}

traffic_light Database::sqlToModTrafficL(QSqlRecord r)
{
    traffic_light out;
    out.traffic_light_id = r.value("TRAFFIC_LIGHT_ID").toInt();
    out.junction_id = r.value("JUNCTION_ID").toInt();
    out.road_ids = r.value("ROAD_IDS").toString();

    return out;
}

QString Database::idsToString(QList<QString> roads)
{
    QString out = QString();
    for(QString i : roads){
        QString id = i.split(',')[0];
        out.append(id);
        out.append(',');
    }

    out.chop(1);
    return out;
}

QList<qint64> Database::stringToIds(QString idsStr)
{
    QList<qint64> ids;
    for(QString i : idsStr.split(',')){
        if(i != ""){
            ids.append(i.toInt());
        }
    }
    return ids;
}

QString Database::anglesToString(QList<QString> roads)
{
    QString out = QString();
    for(QString i : roads){
        QString degree = i.split('|')[1].split(' ')[0];
        out.append(degree);
        out.append(',');
    }

    out.chop(1);
    return out;
}

QList<qint64> Database::stringToAngles(QString anglesStr)
{
    QList<qint64> angls;
    for(QString i : anglesStr.split(',')){
        angls.append(i.toInt());
    }
    return angls;
}

QString Database::addIdsToString(QList<qint64> ids, QString original)
{
//    if(original != ""){
//        ids.append(stringToIds(original));
//    }
    QList<QString> idStr;
    for(qint64 i : ids){
        if(!stringToIds(original).contains(i)){
            qDebug() << "Adding " << i << " to " << original;
            idStr.append(QString("%1").arg(i));
        }else{
            qDebug() << i << " already exists in " << original;
        }
    }
    return idsToString(idStr);
}

bool Database::isIn(qint64 id, QString original)
{
    if(stringToIds(original).contains(id))
        return true;
    return false;
}

mut_road Database::getMeanMutRd(QList<mut_road> mutRds)
{
    mut_road out(mutRds.first().road_id);
    for(mut_road i : mutRds){
        out.vehicle_count += i.vehicle_count;
        out.vehicle_speed += i.vehicle_speed;
        out.congestion_count += i.congestion_count;
    }
    out.vehicle_count /= mutRds.size();
    out.vehicle_speed /= mutRds.size();
    out.congestion_count /= mutRds.size();

    return out;
}

mut_traffic_light Database::getMeanMutTl(QList<mut_traffic_light> mutTls)
{
    mut_traffic_light out(mutTls.first().traffic_light_id);
    for(mut_traffic_light i : mutTls){
        out.green_timer += i.green_timer;
        out.use_count += i.use_count;
    }
    out.green_timer /= mutTls.size();
    out.use_count /= mutTls.size();

    return out;
}

mut_road Database::onPumpRoad(qint64 id)
{
    mut_road out(id);
    if(getAllMutRoads().uniqueKeys().contains(id)){
        QList<mut_road> outList = getAllMutRoads().values(id);
        qDebug() << "SIZEOF MUT_ROAD: " << outList.size();
        for(mut_road i : outList){
            if(i.id > out.id){
                out = i;
            }
        }
    }else{
        qDebug() << "MUT_ROAD NOT FOUND _________________";
    }
    qDebug() << "MUT_ROAD: " << out.road_id << " VEHICLE COUNT: " << out.vehicle_count << " VEHICLE SPEED: " << out.vehicle_speed << " CONGESTION COUNT: " << out.congestion_count << " -ID: " << out.id;
    return out;
}

mut_traffic_light Database::onPumpTrafficLight(qint64 id)
{
    QList<mut_traffic_light> out = getAllMutTraffLsById(id, 1);
    if(out.size() > 0){

    }else{
        qDebug() << "MUT_TL NOT FOUND ___________________";
        out.append(mut_traffic_light(id));
    }
    qDebug() << "MUT_TL: " << out.first().traffic_light_id << " GREEN TIMER: " << out.first().green_timer << " USE COUNT: " << out.first().use_count << " -ID: " << out.first().id;
    return out.first();
}

bool Database::onDumpRoad(QList<mut_road> m_roads)
{
    if(m_roads.size() > 0){
        if(q->prepare("INSERT INTO MUT_ROAD (ROAD_ID, VEHICLE_COUNT, VEHICLE_SPEED, CONGESTION_COUNT, TIMESTAMP)"
                      "VALUES(?,?,?,?,?);")){
            QVariantList rIdList, vCountList, vSpeedList, vCongCount, tsList;
            for(mut_road i : m_roads){
                rIdList << i.road_id;
                vCountList << i.vehicle_count;
                vSpeedList << i.vehicle_speed;
                vCongCount << i.congestion_count;
                tsList << i.timestamp;
            }
            q->addBindValue(rIdList);
            q->addBindValue(vCountList);
            q->addBindValue(vSpeedList);
            q->addBindValue(vCongCount);
            q->addBindValue(tsList);
            if(!q->execBatch()){
                db.rollback();
                emit dbInfo("ERROR: Inserting onDumpRoad FAILED!", ERROR);
                return false;
            }else{
                mutRoadModel->select();
                emit dbChanged();
                emit dbInfo("INFO: Inserting onDumpRoad SUCCESFFULLY!");
                return true;
            }
        }else{
            db.rollback();
            emit dbInfo("ERROR: Preparing onDumpRoad query", ERROR);
        }
    }
    return false;
}

bool Database::onDumpTrafficLight(QList<mut_traffic_light> m_tls)
{
    if(m_tls.size() > 0){
        if(q->prepare("INSERT INTO MUT_TRAFFIC_LIGHT (TRAFFIC_LIGHT_ID, GREEN_TIMER, USE_COUNT, TIMESTAMP)"
                      "VALUES(?,?,?,?);")){
            QVariantList tlIdList, gtList, useList, tsList;
            for(mut_traffic_light i : m_tls){
                tlIdList << i.traffic_light_id;
                gtList << i.green_timer;
                useList << i.use_count;
                tsList << i.timestamp;
            }
            q->addBindValue(tlIdList);
            q->addBindValue(gtList);
            q->addBindValue(useList);
            q->addBindValue(tsList);
            if(!q->execBatch()){
                db.rollback();
                emit dbInfo("ERROR: Inserting onDumpTrafficLight FAILED!", ERROR);
                return false;
            }else{
                mutTrafficLightModel->select();
                emit dbChanged();
                emit dbInfo("INFO: Inserting onDumpTrafficLIght SUCCESFFULLY!");
                return true;
            }
        }else {
            db.rollback();
            emit dbInfo("ERROR: Preparing onDumpTrafficLight query ("+q->lastError().text()+")", ERROR);
        }
    }
    return false;
}
