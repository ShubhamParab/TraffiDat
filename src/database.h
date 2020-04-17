#ifndef DATABASE_H
#define DATABASE_H
#include <QObject>
#include <QMap>
#include <QMultiMap>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>
#include <QPointer>
#include <QWeakPointer>
#include <QDebug>
#include <algorithm>

#include "model.h"
#include "constants.h"

#define VEHICLE_LENGTH 4.8

namespace TraffiDat{

class Database : public QObject
{
    Q_OBJECT
public:
    Database(QString filename, QObject *parent = nullptr);
    void buildMeanValues();
    mut_road getMeanMutRoad(QList<mut_road> roads);
    bool addRoad(road r);
    bool addTrafficLight(traffic_light tl);
    bool removeTrafficLight(traffic_light tl);
    bool removeTrafficLightByJunctionId(qint64 id);
    bool replaceTrafficLight(traffic_light tl);
    bool addJunction(junction j);
    bool removeJunction(junction j);
    void updateRoad(road r);
    void updateTrafficLight(traffic_light tl);
    void updateJunction(junction j);
    QList<road> getAvailRoads();
    road roadById(qint64 id);
    QList<road> roadsByJunction(junction j);
    QList<junction> getAvailJuncts();
    junction junctionById(qint64 id);
    QList<traffic_light> getAvailTraffL();
    QList<qint64> getUnallocTraffL(qint64 value);
    traffic_light trafficLightById(qint64 id);
    QList<traffic_light> trafficLightsByJunctionId(qint64 id);
    QList<traffic_light> trafficLightByJunctionIdNRoadId(qint64 junctId, qint64 rdId);
    mut_road getMeanMutRdById(qint64 id = 0){return meanMutRds.value(id);}
    QMap<qint64, mut_road> getMeanMutRds(){return meanMutRds;}
    QMultiMap<qint64, mut_road> getAllMutRoads();
    QList<mut_road> getAllMutRoadsById(qint64 id, qint64 sampleCount = -1);
    bool removeMutRoad(mut_road mr);
    QMultiMap<qint64, QPair<qint64, mut_traffic_light>> getAllMutTraffLs();
    QList<mut_traffic_light> getAllMutTraffLsById(qint64 id, qint64 sampleCount = -1);
    bool removeMutTl(mut_traffic_light mtl);
    void saveAll();
    ~Database();

    static mut_road sqlToModMutRoad(QSqlRecord r);
    static QSqlRecord modToSqlMutRoad(mut_road r);
    static QSqlRecord modToSqlRoad(road r);
    static mut_traffic_light sqlToModMutTraffL(QSqlRecord r);
    static QSqlRecord modToSqlMutTraffL(mut_traffic_light r);

    static road sqlToModRoad(QSqlRecord r);
    static junction sqlToModJunction(QSqlRecord r);
    static traffic_light sqlToModTrafficL(QSqlRecord r);

    static QString idsToString(QList<QString> roads);
    static QList<qint64> stringToIds(QString idsStr);
    static QString anglesToString(QList<QString> roads);
    static QList<qint64> stringToAngles(QString anglesStr);

    static QString addIdsToString(QList<qint64> ids, QString original);
    static bool isIn(qint64 id, QString original);

    static mut_road getMeanMutRd(QList<mut_road> mutRds);
    static mut_traffic_light getMeanMutTl(QList<mut_traffic_light> mutTls);

signals:
    void dbInfo(QString msg, qint64 styleType = 1);
    void dbChanged();

public slots:
    mut_road onPumpRoad(qint64 id = -1);  // [DEPRECIATED]
    mut_traffic_light onPumpTrafficLight(qint64 id = 1); // [DEPRECIATED]
    bool onDumpRoad(QList<mut_road> m_roads);
    bool onDumpTrafficLight(QList<mut_traffic_light> m_tls);

private:
    QString filename;
    QSqlDatabase db;
    QSqlQuery *q = nullptr;
    QSqlTableModel *mutRoadModel = nullptr;
    QSqlTableModel *mutTrafficLightModel = nullptr;
    QSqlTableModel *roadModel = nullptr;
    QSqlTableModel *trafficLightModel = nullptr;
    QSqlTableModel *junctionModel = nullptr;

    QMap<qint64, mut_road> meanMutRds;
};

}

#endif // DATABASE_H
