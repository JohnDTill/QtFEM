#ifndef NODE_H
#define NODE_H

#include <QGraphicsItem>

class MainWindow;

class Node : public QGraphicsItem
{
public:
    double x = 0;
    double y = 0;
    const double scale_factor = 300;
    int id;
    bool prescribed_x = false;
    bool prescribed_y = false;
    bool prescribed_3 = false;
    double val_x = 0;
    double val_y = 0;
    double val_3 = 0;

private:
    bool is_selected = true;
    MainWindow* mw;

public:
    Node(MainWindow* mw, int id);
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void select();
    void deselect();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // NODE_H
