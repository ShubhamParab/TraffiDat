#ifndef JUNCTIONGRAPHICSITEM_H
#define JUNCTIONGRAPHICSITEM_H
#include <QGraphicsItem>
#include <QPainter>
#include <QRandomGenerator>
#include <QPen>

#include "math.h"
#include "model.h"
//#define M_PI 3.14159265358979323846

using namespace TraffiDat;
class JunctionGraphicsItem : public QGraphicsItem
{
public:
    JunctionGraphicsItem(junction j, int x = 0, int y = 0);
    ~JunctionGraphicsItem() override;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QPen getPen() { return pen;}
    junction getJunct() {return mainJunction;}
    QPainterPath shape() const override;

private:
    junction mainJunction;
    int center_x, center_y;
    qreal radius;

    QPen pen;
};

#endif // JUNCTIONGRAPHICSITEM_H
