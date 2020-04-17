#ifndef ROADGRAPHICSITEM_H
#define ROADGRAPHICSITEM_H
#include <QGraphicsItem>
#include <QRandomGenerator>
#include <QPen>
#include <QtMath>
#include <QPainter>
#include <QPolygonF>
#include <QPointer>
#include <QDebug>

#include "model.h"
#include "constants.h"

using namespace TraffiDat;

enum ROAD_FLOW {INCOMING, OUTGOING};

class RoadGraphicsItem : public QGraphicsItem
{
public:
    RoadGraphicsItem(road r, mut_road *mutRoad = new mut_road(-1), qint64 angle = 10, ROAD_FLOW rf = INCOMING, float *p = new float(0), bool *congRd = new bool(false), qint64 *traffL = new qint64(NO_SIGNAL), qint64 x = 0, qint64 y = 0);
    ~RoadGraphicsItem();

    void setTraffL(qint64 *value){traffL = value;}

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QPen getPen() {return pen;}
    road getRoad() {return mainRoad;}
    QPainterPath shape() const override;
    QPolygonF getArrowAt(QPointF pt = QPointF(0, 0));
    QColor getColor(qreal p = -1);
    float getP(){return *p;}
    QColor getTraffLColor();

public slots:
    void setP(qreal value){*p = value;}

private:
    road mainRoad;
    mut_road *mutRoad = nullptr;
    float center_x, center_y;
    float *p;
    bool *congRd;
    qint64 *traffL = nullptr;
    qreal width, height;
    int angle;
    ROAD_FLOW rf;

    QPen pen;
    QLineF *mainLine = nullptr;    // road bone

};

#endif // ROADGRAPHICSITEM_H
