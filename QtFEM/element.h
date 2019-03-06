#ifndef ELEMENT_H
#define ELEMENT_H

#include <QGraphicsItem>
#include <node.h>

//No click and drag with the elements is the easy way out!

class Element : public QGraphicsItem
{

private:
    Node* left;
    Node* right;
    QPointF sourcePoint;
    QPointF destPoint;

public:
    Element(Node& sourceNode, Node& destNode);
    void adjust();

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // ELEMENT_H
