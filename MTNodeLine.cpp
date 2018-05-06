#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include "MTNodeLine.h"

MTNodeLine::MTNodeLine(const QPointF &a, const QPointF &b, QGraphicsItem *parent)
: QGraphicsPathItem(parent)
, pointA(a)
, pointB(b)
, source(Q_NULLPTR)
, target(Q_NULLPTR)
{
    setZValue(-1);
    setBrush(Qt::NoBrush);
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(2);
    pen.setColor(QColor(255, 20, 20));
    setPen(pen);

    updatePath();
}

MTNodeLine::~MTNodeLine(void) {

}

int MTNodeLine::type(void) const { return Type; }

void MTNodeLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (painter == Q_NULLPTR) {
        return;
    }

    painter->setPen(pen);
    painter->drawPath(path());
}

void MTNodeLine::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    setPointB(event->pos());
}

void MTNodeLine::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    setPointB(event->pos());
}

void MTNodeLine::updatePath(void) {
    QPainterPath path;
    path.moveTo(pointA);

    qreal dx = pointB.x() - pointA.x();
    qreal dy = pointB.y() - pointB.y();

    QPointF ctrl1(pointA.x() + dx * 0.25, pointA.y() + dy * 0.1);
    QPointF ctrl2(pointA.x() + dx * 0.75, pointA.y() + dy * 0.9);

    path.cubicTo(ctrl1, ctrl2, pointB);
    setPath(path);
}
