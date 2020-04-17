#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <QString>
#include <QTranslator>
#include <QDateTime>
#include <QDebug>
#include <QtMath>

#include <qwt_text.h>

namespace TraffiDat{

typedef struct road{
    qint64 road_id;
    QString name;
    float length;
    qint64 lanes;
    float perc_damage;
    QString video_addr;

    inline bool operator==(road r){
        if(road_id == r.road_id)
            return true;
        else
            return false;
    }
}road;

enum rPlot{
    RPLOT_ID,
    RPLOT_VEHICLE_COUNT,
    RPLOT_VEHICLE_SPEED,
    RPLOT_CONGESTION_COUNT,
    RPLOT_TIMESTAMP
};

typedef struct mut_road{
    qint64 id;
    qint64 road_id;
    qint64 vehicle_count;
    float vehicle_speed;
    qint64 congestion_count;
    QString timestamp;

    mut_road(qint64 road_id):id(-1), road_id(road_id), vehicle_count(0), vehicle_speed(0), congestion_count(0), timestamp(QString(QDateTime::currentDateTime().toString(Qt::ISODate))){

    }

    mut_road(){
        mut_road(-1);
    }

    inline bool operator==(mut_road mr){
        if(road_id == mr.road_id)
            return true;
        else
            return false;
    }
}mut_road;

typedef struct traffic_light{
    qint64 traffic_light_id;
    qint64 junction_id;
    QString road_ids;

    traffic_light(qint64 traffic_light_id, qint64 junction_id): traffic_light_id(traffic_light_id), junction_id(junction_id), road_ids(QString("")){

    }

    traffic_light(qint64 traffic_light_id): traffic_light_id(traffic_light_id), junction_id(-1), road_ids(QString("")){

    }

    traffic_light(){
        traffic_light(-1);
    }

    inline bool operator==(traffic_light tl){
        if(traffic_light_id == tl.traffic_light_id)
            return true;
        else
            return false;
    }
}traffic_light;

enum tlPlot{
    TLPLOT_ID,
    TLPLOT_GREEN_TIMER,
    TLPLOT_USE_COUNT,
    TLPLOT_TIMESTAMP
};

typedef struct mut_traffic_light{
    qint64 id;
    qint64 traffic_light_id;
    qint64 green_timer;
    qint64 use_count;
    QString timestamp;

    mut_traffic_light(qint64 traffic_light_id):id(-1), traffic_light_id(traffic_light_id), green_timer(10), use_count(0), timestamp(QDateTime::currentDateTime().toString(Qt::ISODate)){

    }

    mut_traffic_light(qint64 traffic_light_id, qint64 green_timer):id(-1), traffic_light_id(traffic_light_id), green_timer(green_timer), use_count(0), timestamp("TIME"){

    }
    mut_traffic_light(){
        mut_traffic_light(-1);
    }

    inline bool operator==(mut_traffic_light mtl){
        if(traffic_light_id == mtl.traffic_light_id)
            return true;
        else
            return false;
    }
}mut_traffic_light;

typedef struct junction{
    qint64 junction_id;
    QString name;
    float area;
    QString up_road_ids;
    QString up_road_angles;
    QString down_road_ids;
    QString down_road_angles;

    junction(qint64 junction_id):junction_id(junction_id), name(), area(0), up_road_ids(), up_road_angles(), down_road_ids(), down_road_angles(){

    }

    junction():junction_id(-1), name(), area(0), up_road_ids(), up_road_angles(), down_road_ids(), down_road_angles(){

    }

    inline bool operator==(junction j){
        if(junction_id == j.junction_id)
            return true;
        else
            return false;
    }
}junction;

inline void setMutRoadVehCount(mut_road* mutRd, qint64 value){mutRd->vehicle_count = value;}
inline void setMutRoadVehSpeed(mut_road* mutRd, qint64 value){mutRd->vehicle_speed = value;}
inline void setMutRoadCongCount(mut_road* mutRd, qint64 value){mutRd->congestion_count = value;}
inline void setMutRoadTimestamp(mut_road* mutRd, QString value = QDateTime::currentDateTime().toString(Qt::ISODate)){mutRd->timestamp = value;}

inline void setMutTraffLGreenTimer(mut_traffic_light* mutTl, qint64 value){mutTl->green_timer = value;}
inline void setMutTraffLUseCount(mut_traffic_light* mutTl, qint64 value){mutTl->use_count = value;}
inline void setMutTraffLTimestamp(mut_traffic_light* mutTl, QString value = QDateTime::currentDateTime().toString(Qt::ISODate)){mutTl->timestamp = value;}

inline QString repr_road(road i){
    return QString("%1,%2,%3").arg(i.road_id).arg(i.name).arg(i.video_addr);
}

inline QString repr_junction(junction j){
    return QString("%1,%2").arg(j.junction_id).arg(j.name);
}

inline QString repr_traffiL(traffic_light tl){
    return QString("%1,%2,%3").arg(tl.traffic_light_id).arg(tl.junction_id).arg(tl.road_ids);
}

inline QString repr_traffLs(QList<traffic_light> tl){
    QList<QString> ids;
    for(traffic_light i : tl){
        ids.append(QString("%1").arg(i.traffic_light_id));
    }
    QStringList idsStr = QStringList(ids);
    return idsStr.join(",");
}

inline QString repr_mut_road(mut_road r){
    return QString("%1,%2,%3,%4,%5").arg(r.id).arg(r.road_id).arg(r.vehicle_count).arg(r.vehicle_speed).arg(r.timestamp);
}

inline QString repr_mut_traffic_light(mut_traffic_light tl){
    return QString("%1,%2,%3").arg(tl.traffic_light_id).arg(tl.green_timer).arg(tl.timestamp);
}

inline qreal getTrafficDensity(road r, mut_road mR, qreal error = 0.0){
    Q_UNUSED(error);
//    qreal total_road = r.length*r.lanes;
//    qreal damage = (r.perc_damage/100)*total_road;
//    qreal dst = mR.vehicle_count/(total_road-damage);
//    qDebug() << "ROAD: " << r.road_id << " DENSITY: " << dst;
//    return dst;
    return mR.vehicle_count;
}
inline qreal getTrafficBreakdownProbability(qreal x, qreal mean_x = 40, qreal slope = 0.066){
//     qDebug() << " X: " << x << " MEAN_X: " << mean_x;
    qreal expo = qPow(M_E, -slope*(x-mean_x));
    qreal dnom = 1+expo;
    qreal out = 1/dnom;
//     qDebug() << " P: " << out;
    return out;
}

// LOGIC
inline bool is_diverging(qint64 r11, qint64 r12, qint64 r21, qint64 r22){
    if((r11 == r21) && (r12 != r22))
        return true;
    else
        return false;
}

inline bool is_converging(qint64 r11, qint64 r12, qint64 r21, qint64 r22){
    if((r11 != r21) && (r12 == r22))
        return true;
    else
        return false;
}

inline bool is_in(qint64 angle, qint64 lower, qint64 upper){
    if((angle >= lower) && (angle <=  upper))
        return true;
    else
        return false;
}

inline bool is_crossing(qint64 r11, qint64 r12, qint64 r21, qint64 r22){
    qint64 lower1 = qMin(r11, r12);
    qint64 upper1 = qMax(r11, r12);
    qint64 lower2 = qMin(r21, r22);
    qint64 upper2 = qMin(r21, r22);
    if(is_in(lower1, lower2, upper2) != is_in(upper1, lower2, upper2) || (is_in(lower2, lower1, upper1) != is_in(upper2, lower1, upper1)))
        return true;
    else
        return false;
}

static bool isConvergingAllowed = true;
static bool isDivergingAllowed = true;
static bool isCrossingAllowed = true;

inline bool is_compatible(qint64 r11, qint64 r12, qint64 r21, qint64 r22, qreal p11, qreal p21){
    qDebug() << "angle11:"  << r11 << " angle12: " << r12 << " angle21: " << r21 << " angle22: " << r22;
    qDebug() << "p11: " << p11 << " AND " << " p21: " << p21;
    if(is_converging(r11, r12, r21, r22) && isConvergingAllowed){
        if(qMax(p11, p21) > 0.8){
            qDebug() << "CONVERGING";
            return true;
        }else{
            qDebug() << "CONVERGING -NOT COMPATIBLE";
            return false;
        }
    }else if(is_diverging(r11, r12, r21, r22) && isDivergingAllowed){
        if(p11 > 0.6){
            qDebug() << "DIVERGING";
            return true;
        }else{
            qDebug() << "DIVERGING -NOT COMPATIBLE";
            return false;
        }
    }else if(is_crossing(r11, r12, r21, r22) && isCrossingAllowed){
        qDebug() << "CROSSING";
        return false;
    }else if(!is_crossing(r11, r12, r21, r22)){
        qDebug() << "NOT CROSSING";
        return true;
    }else {
        qDebug() << "NOT COMPATIBLE";
        return false;
    }
}

// GRAPH COLOURING

class GraphColouring{
public:
    GraphColouring(QList<road> upRds, QList<qint64> upAngles, QList<road> downRds, QList<qint64> downAngles, QMap<qint64, float*> probabs);
    void printSolution(int color[]);
    bool isSafe (int v, int color[], int c);
    bool graphColoringUtil(int m, int color[], int v);
    bool graphColoring(int m);

    QList<QPair<qint64, qint64>> getRoutes(){return routes;}
    int* getColor(){return color;}

private:
    QList<QPair<qint64, qint64>> routes;
    QList<QPair<qint64, qint64>> angles;

    int V;
    QVector<QVector<qint64>> graph;
    int* color;

};

}

double getMin(QVector<double> ls);
double getMax(QVector<double> ls);

#endif // MODEL_H
