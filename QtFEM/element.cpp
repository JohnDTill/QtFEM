#include "element.h"

#include <QPainter>

Element::Element(Node& sourceNode, Node& destNode){
    setAcceptedMouseButtons(0);
    left = &sourceNode;
    right = &destNode;
    adjust();
}

void Element::adjust()
{
    QLineF line(mapFromItem(left, 0, 0), mapFromItem(right, 0, 0));
    qreal length = line.length();

    prepareGeometryChange();

    if (length > qreal(20.)) {
        QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);
        sourcePoint = line.p1() + edgeOffset;
        destPoint = line.p2() - edgeOffset;
    } else {
        sourcePoint = destPoint = line.p1();
    }
}

QRectF Element::boundingRect() const{
    QPointF direction = left->pos() - right->pos();
    return QRectF(sourcePoint, direction).normalized();
}

void Element::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *){
    adjust();

    QLineF line(sourcePoint, destPoint);
    if (qFuzzyCompare(line.length(), qreal(0.)))
        return;

    // Draw the line itself
    painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(line);
}
