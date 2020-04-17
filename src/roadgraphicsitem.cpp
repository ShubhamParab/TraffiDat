#include "roadgraphicsitem.h"

RoadGraphicsItem::RoadGraphicsItem(road r, mut_road *mutRoad, qint64 angle, ROAD_FLOW rf, float *p, bool *congRd, qint64 *traffL, qint64 x, qint64 y):
    mainRoad(r),
    mutRoad(mutRoad),
    p(p),
    center_x(x),
    center_y(y),
    congRd(congRd),
    traffL(traffL),
    angle(angle),
    rf(rf)
{
    pen = QPen();

    mainLine = new QLineF();
    mainLine->setP1(QPointF(center_x+0.1, center_y));

    mainLine->setAngle(this->angle);
    mainLine->setLength(mainRoad.length);

    qDebug() << " P1: " << mainLine->p1().x() << " " << mainLine->p1().y() << ", " << mainLine->p2().x() << " " << mainLine->p2().y() << " P: " << *p;
}

RoadGraphicsItem::~RoadGraphicsItem()
{
//    if(mainLine != nullptr)
//        delete mainLine;
//    if(p != nullptr)
//        delete p;
//    if(mutRoad != nullptr)
//        delete mutRoad;
}

QRectF RoadGraphicsItem::boundingRect() const
{
    return QRectF(mainLine->p1(), mainLine->p2()).normalized();
}

void RoadGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();

    // drawing road
    pen.setColor(getColor(*p));
    painter->setPen(pen);
    painter->drawLine(mainLine->toLine());

    // drawing arrows
    for(qreal i = 0.4; i < 0.8; i+=0.2)
        painter->drawPolygon(getArrowAt(mainLine->pointAt(i)));

    // drawing traffic light
    QPointF traffLPt = mainLine->pointAt(1);
    QBrush b;
    pen.setColor(getTraffLColor());
    b.setColor(getTraffLColor());
    b.setStyle(Qt::BrushStyle::SolidPattern);
    painter->setPen(pen);
    painter->setBrush(b);
    painter->drawEllipse(traffLPt, 2, 2);

    // drawing road details
    pen.setColor(Qt::blue);
    painter->setPen(pen);
    painter->setFont(QFont("Helvetica [Cronyx]", 1));
    QPointF anchor = mainLine->pointAt(0.7);
    if(mainRoad.road_id == 6){
        qDebug() << "ANCHOR X: " << anchor.x() << " Y: " << anchor.y();
    }
    painter->drawText(anchor.x(), anchor.y(), QString("NAME: %1").arg(mainRoad.name));
    painter->drawText(anchor.x(), anchor.y()+3, QString("LENGTH: %1, LANES: %2").arg(mainRoad.length).arg(mainRoad.lanes));
    painter->drawText(anchor.x(), anchor.y()+6, QString("DAMAGE: %1 %").arg(mainRoad.perc_damage));
    painter->drawText(anchor.x(), anchor.y()+9, QString("TRAFFIC DENSITY: %1").arg(QString::number(*p*100)));

    // congestion indicator
    if(*congRd){
        pen.setColor(Qt::red);
        painter->setPen(pen);
        painter->drawText(mainLine->pointAt(0.9), "CONGESTED!");
    }

    painter->restore();
}

QPainterPath RoadGraphicsItem::shape() const
{
    QPainterPath path;
    path.addRect(QRectF(mainLine->p1(), mainLine->p2()).normalized());
    return path;
}

QPolygonF RoadGraphicsItem::getArrowAt(QPointF pt)
{
    QLineF l1;
    l1.setP1(pt);
    l1.setLength(3);
    if(rf == INCOMING)
        l1.setAngle(angle+120+180);
    else
        l1.setAngle(angle+120);

    QLineF l2;
    l2.setP1(l1.p2());
    if(rf == INCOMING)
        l2.setAngle(angle+270+180);
    else
        l2.setAngle(angle+270);
    l2.setLength(6);

    QLineF l3;
    l3.setP1(l2.p2());
    l3.setP2(l1.p1());

    return QPolygonF({l1.p1(), l2.p1(), l3.p1()});
}

QColor RoadGraphicsItem::getColor(qreal p)
{
//    qDebug() << "RENDER P: " << p;
    if(p >= 0 && p < 0.5){
        return QColor((p/0.5)*255, 255, 0);
    }else if(p == 0.5){
        return QColor(255, 255, 0);
    }else if(p > 0.5 && p <= 1.0){
        return QColor(255, ((1-p)/0.5)*255, 0);
    }
    return QColor(Qt::lightGray);
}

QColor RoadGraphicsItem::getTraffLColor()
{
    switch (*traffL) {
    case RED_SIGNAL:
//        qDebug() << "ROAD ID: " << mainRoad.road_id << " SIGNAL: RED ";
        return QColor(Qt::red);
    case YELLOW_SIGNAL:
//        qDebug() << "ROAD ID: " << mainRoad.road_id << " SIGNAL: YELLOW ";
        return QColor(Qt::yellow);
    case GREEN_SIGNAL:
//        qDebug() << "ROAD ID: " << mainRoad.road_id << " SIGNAL: GREEN ";
        return QColor(Qt::green);
    case NO_SIGNAL:
//        qDebug() << "ROAD ID: " << mainRoad.road_id << " SIGNAL: DEAD ";
        return QColor(Qt::black);
    default:
//        qDebug() << "ROAD ID: " << mainRoad.road_id << " SIGNAL: INVALID ";
        return QColor(Qt::gray);
    }
}
