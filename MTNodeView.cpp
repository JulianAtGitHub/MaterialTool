#include <QGraphicsScene>
#include <QMouseEvent>
#include <QWheelEvent>
#include "MTNodeView.h"

MTNodeView::MTNodeView(QWidget *parent)
: QGraphicsView(parent)
, isDragging(false)
{

}

MTNodeView::~MTNodeView(void) {

}

void MTNodeView::initSceneUi(void) {
    QGraphicsScene *scene = new QGraphicsScene();
    scene->setObjectName("scene");
    scene->setSceneRect(0, 0, 10240, 10240);
    setScene(scene);
}

void MTNodeView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        setDragMode(QGraphicsView::NoDrag);
        isDragging = true;
        prevPos = event->pos();
        setCursor(Qt::SizeAllCursor);
    } else if (event->button() == Qt::LeftButton) {
        setDragMode(QGraphicsView::RubberBandDrag);
    }

    QGraphicsView::mousePressEvent(event);
}

void MTNodeView::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging) {
        QPointF delta = (mapToScene(event->pos()) - mapToScene(prevPos)) * -1.0;
        QPointF center = mapToScene(viewport()->width() / 2.0 + delta.x(), viewport()->height() / 2.0 + delta.y());
        centerOn(center);
        prevPos = event->pos();
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void MTNodeView::mouseReleaseEvent(QMouseEvent *event) {
    if (isDragging) {
        isDragging = false;
        setCursor(Qt::ArrowCursor);
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void MTNodeView::wheelEvent(QWheelEvent *event) {
    if (isDragging) {
        return;
    }

    qreal inFactor = 1.25;
    qreal outFactor = 1.0 / inFactor;
    qreal zoomFactor = inFactor;
    if (event->delta() <= 0) {
        zoomFactor = outFactor;
    }

    QPointF oldPos = mapToScene(event->pos());
    scale(zoomFactor, zoomFactor);
    QPointF newPos = mapToScene(event->pos());
    QPointF delta = newPos - oldPos;
    translate(delta.x(), delta.y());
}
