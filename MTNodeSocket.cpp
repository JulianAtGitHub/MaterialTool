#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include "MTNodeSocket.h"
#include "MTNodeLine.h"
#include "MTNodeItem.h"

MTNodeSocket::MTNodeSocket(Mode t, QRect r, QGraphicsItem *parent)
: QGraphicsItem(parent)
, mode(t)
, rect(r)
, newLine(Q_NULLPTR)
{
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(QColor(180, 20, 90));

    pen.setStyle(Qt::SolidLine);
    pen.setWidth(1);
    pen.setColor(QColor(20, 20, 20));
}

MTNodeSocket::~MTNodeSocket(void) {

}

int MTNodeSocket::type(void) const { return Type; }

QPainterPath MTNodeSocket::shape(void) const {
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
}

QRectF MTNodeSocket::boundingRect(void) const {
    return QRectF(rect);
}

void MTNodeSocket::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(brush);
    painter->setPen(pen);
    painter->drawEllipse(rect);
}

QPointF MTNodeSocket::getCenter(void) {
    QRectF rect = boundingRect();
    QPointF center(rect.x() + rect.width()/2.0, rect.y() + rect.height()/2.0);
    center = mapToScene(center);
    return center;
}

void MTNodeSocket::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    switch(mode) {
        case Out: {
            QRectF rect = boundingRect();
            QPointF pointA(rect.x() + rect.width()/2.0, rect.y() + rect.height()/2.0);
            pointA = mapToScene(pointA);
            QPointF pointB = mapToScene(event->pos());
            newLine = new MTNodeLine(pointA, pointB);
            outLines.append(newLine);
            scene()->addItem(newLine);
            break;
        }
        case In: {
            QRectF rect = boundingRect();
            QPointF pointA = mapToScene(event->pos());
            QPointF pointB(rect.x() + rect.width()/2.0, rect.y() + rect.height()/2.0);
            pointB = mapToScene(pointB);
            newLine = new MTNodeLine(pointA, pointB);
            inLines.append(newLine);
            scene()->addItem(newLine);
            break;
        }
        default:
            QGraphicsItem::mousePressEvent(event);
            break;
    }
}

void MTNodeSocket::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    switch(mode) {
        case Out: {
            QPointF pointB = mapToScene(event->pos());
            newLine->setPointB(pointB);
            break;
        }
        case In: {
            QPointF pointA = mapToScene(event->pos());
            newLine->setPointA(pointA);
            break;
        }
        default:
            QGraphicsItem::mouseMoveEvent(event);
            break;
    }
}

void MTNodeSocket::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    do {
        QGraphicsItem *item = scene()->itemAt(event->scenePos(), QTransform());
        if (item == Q_NULLPTR) {
            break;
        }

        MTNodeSocket *socket = qgraphicsitem_cast<MTNodeSocket *>(item);
        if (socket == Q_NULLPTR) {
            break;
        }

        if (mode == Out && socket->mode == In) {
            newLine->setSource(this);
            newLine->setTarget(socket);
            MTNodeItem *parent = qgraphicsitem_cast<MTNodeItem *>(socket->parentItem());
            parent->getInput().getInLines().append(newLine);
            newLine->setPointB(socket->getCenter());
            newLine = Q_NULLPTR;
            return;

        } else if (mode == In && socket->mode == Out) {
            newLine->setSource(socket);
            newLine->setTarget(this);
            MTNodeItem *parent = qgraphicsitem_cast<MTNodeItem *>(socket->parentItem());
            parent->getOutput().getOutLines().append(newLine);
            newLine->setPointA(socket->getCenter());
            newLine = Q_NULLPTR;
            return;
        }

    } while (false);

    QGraphicsItem::mouseReleaseEvent(event);
}
