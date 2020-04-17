#include "junctiongraphicsitem.h"

JunctionGraphicsItem::JunctionGraphicsItem(junction j, int x, int y):mainJunction(j), center_x(x), center_y(y)
{
    radius = std::sqrt(mainJunction.area/M_PI);
    pen = QPen(Qt::gray);
}

JunctionGraphicsItem::~JunctionGraphicsItem()
{

}

QRectF JunctionGraphicsItem::boundingRect() const
{
    return QRectF(QPointF(center_x-radius-10, center_y-radius-10), QPointF(center_x+2*radius+20, center_y+2*radius+10));
}

void JunctionGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->save();

    painter->setFont(QFont("Helvetica [Cronyx]", 1));
    painter->setPen(pen);
    painter->drawPoint(QPointF(center_x, center_y));

    QBrush b;
    b.setColor(Qt::gray);
    b.setStyle(Qt::BrushStyle::SolidPattern);
    painter->setBrush(b);
    painter->drawEllipse(QPointF(center_x, center_y), radius, radius);

    QPen refPen = QPen(Qt::GlobalColor::darkMagenta);
    refPen.setStyle(Qt::PenStyle::DotLine);

    painter->setPen(refPen);
    painter->drawLine(center_x, center_y, center_x+50, center_y);
    painter->drawText(center_x+15, center_y+4, QString("Ref. Line"));
    painter->drawText(center_x-7, center_y-2, QString("JUNCTION: %1").arg(mainJunction.name));
    painter->drawText(center_x-7, center_y-4, QString("AREA: %1 sq-km").arg(mainJunction.area));
    painter->restore();
}

QPainterPath JunctionGraphicsItem::shape() const
{
    QPainterPath path;
    path.addEllipse(QPointF(center_x, center_y), radius, radius);
    return path;
}
