#ifndef FEMVIEW_H
#define FEMVIEW_H

#include <QGraphicsView>

class FemView : public QGraphicsView
{
    Q_OBJECT
public:
    QGraphicsScene *scene = new QGraphicsScene(this);

public:
    FemView(QWidget *parent = 0);
    void addItem(QGraphicsItem* item){scene->addItem(item);}
    void scaleView(qreal scaleFactor);

protected:
    void keyPressEvent(QKeyEvent *event) override;
#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent *event) override;
#endif
    void drawBackground(QPainter *painter, const QRectF &rect) override;
};

#endif // FEMVIEW_H
