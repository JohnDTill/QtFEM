#include "node.h"

#include <QPainter>
#include <QStyleOption>

#include <mainwindow.h>

Node::Node(MainWindow* mw, int id){
    this->mw = mw;
    this->id = id;
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);
}

QRectF Node::boundingRect() const{
    qreal adjust = 2;
    return QRectF( -10 - adjust, -10 - adjust, 23 + adjust, 23 + adjust);
}

QPainterPath Node::shape() const{
    QPainterPath path;
    path.addEllipse(-10, -10, 20, 20);
    return path;
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *){
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::darkGray);
    painter->drawEllipse(-7, -7, 20, 20);

    QRadialGradient gradient(-3, -3, 10);
    if (is_selected) {
        gradient.setCenter(3, 3);
        gradient.setFocalPoint(3, 3);
        gradient.setColorAt(1, QColor(QColor::fromRgb(255,128,0)).light(120));
        gradient.setColorAt(0, QColor::fromRgb(215,98,0).light(120));
    } else {
        gradient.setColorAt(0, QColor::fromRgb(255,128,0));
        gradient.setColorAt(1, QColor::fromRgb(215,98,0));
    }
    painter->setBrush(gradient);

    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(-10, -10, 20, 20);
}

void Node::select(){
    is_selected = true;
    update();
}

void Node::deselect(){
    is_selected = false;
    update();
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value){
    switch (change) {
    case ItemPositionHasChanged:
        //foreach (Edge *edge, edgeList)
        //    edge->adjust();
        //graph->itemMoved();
        break;
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event){
    mw->selectNode(this);
    update();
    QGraphicsItem::mousePressEvent(event);
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    QPointF p = pos();
    x = p.rx()/scale_factor;
    y = -p.ry()/scale_factor;
    mw->updateNodeCoordinates();

    update();
    QGraphicsItem::mouseReleaseEvent(event);
}
