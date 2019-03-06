#include "femview.h"

#include <QKeyEvent>

FemView::FemView(QWidget *parent) : QGraphicsView(parent){
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    //scene->setSceneRect(-2000, -2000, 4000, 4000);
    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(ViewportUpdateMode::FullViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    //scale(qreal(1.2), qreal(1.2));
    //setMinimumSize(400, 400);

    setDragMode(QGraphicsView::ScrollHandDrag);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

void FemView::keyPressEvent(QKeyEvent *event){
    switch (event->key()) {
    case Qt::Key_Up:
        //centerNode->moveBy(0, -20);
        break;
    case Qt::Key_Down:
        //centerNode->moveBy(0, 20);
        break;
    case Qt::Key_Left:
        //centerNode->moveBy(-20, 0);
        break;
    case Qt::Key_Right:
        //centerNode->moveBy(20, 0);
        break;
    case Qt::Key_Space:
    case Qt::Key_Enter:
        //shuffle();
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}

#if QT_CONFIG(wheelevent)
void FemView::wheelEvent(QWheelEvent *event){
    scaleView(pow((double)2, event->delta() / 240.0));
}
#endif

void FemView::drawBackground(QPainter *painter, const QRectF &rect){
    Q_UNUSED(rect);
}

void FemView::scaleView(qreal scaleFactor){
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.005 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}
