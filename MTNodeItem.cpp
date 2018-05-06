#include <QtGlobal>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QMenu>
#include <QAction>
#include "MTNodeItem.h"
#include "MTNodeSocket.h"
#include "MTNodeLine.h"

MTNodeItem::MTNodeItem(QGraphicsItem *parent)
: QGraphicsItem(parent)
, rect(0, 0, 100, 60)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);

    input = new MTNodeSocket(MTNodeSocket::In, QRect(-10, 20, 20, 20), this);
    output = new MTNodeSocket(MTNodeSocket::Out, QRect(90, 20, 20, 20), this);

    brush.setStyle(Qt::SolidPattern);
    brush.setColor(QColor(80, 0, 90));

    pen.setStyle(Qt::SolidLine);
    pen.setWidth(1);
    pen.setColor(QColor(20, 20, 20));

    selPen.setStyle(Qt::SolidLine);
    selPen.setWidth(1);
    selPen.setColor(QColor(0, 255, 255));
}

MTNodeItem::~MTNodeItem(void) {

}

int MTNodeItem::type(void) const { return Type; }

QPainterPath MTNodeItem::shape(void) const {
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
}

QRectF MTNodeItem::boundingRect(void) const {
    return QRectF(rect);
}

void MTNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(brush);
    if (isSelected()) {
        painter->setPen(selPen);
    } else {
        painter->setPen(pen);
    }
    painter->drawRect(rect);
}

void MTNodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent(event);
    foreach (MTNodeLine *line, output->getOutLines()) {
        line->setPointA(line->getSource()->getCenter());
        line->setPointB(line->getTarget()->getCenter());
    }
    foreach (MTNodeLine *line, input->getInLines()) {
        line->setPointA(line->getSource()->getCenter());
        line->setPointB(line->getTarget()->getCenter());
    }
}

void MTNodeItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    QMenu menu;
    QAction *make = menu.addAction("make");
    QAction *makeFromHere = menu.addAction("make from here");
    QAction *debugConnections = menu.addAction("debug connections");
    QAction *selectedAction = menu.exec(event->screenPos());
    if (selectedAction == debugConnections) {
        qInfo("debug output");
    }

    Q_UNUSED(make);
    Q_UNUSED(makeFromHere);
}

