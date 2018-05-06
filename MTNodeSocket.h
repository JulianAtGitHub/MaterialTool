#ifndef MTNODESOCKET_H
#define MTNODESOCKET_H

#include <QGraphicsItem>
#include <QRect>
#include <QBrush>
#include <QPen>
#include <QList>

class MTNodeLine;

class MTNodeSocket : public QGraphicsItem {

public:
    enum Mode {
        In,
        Out
    };

    explicit MTNodeSocket(Mode t, QRect r, QGraphicsItem *parent = Q_NULLPTR);
    virtual ~MTNodeSocket(void);

    virtual QPainterPath shape(void) const Q_DECL_OVERRIDE;
    virtual QRectF boundingRect(void) const Q_DECL_OVERRIDE;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) Q_DECL_OVERRIDE;

    QPointF getCenter(void);

    inline Mode getMode(void) const { return mode; }
    inline QList<MTNodeLine *> & getInLines(void) { return inLines; }
    inline QList<MTNodeLine *> & getOutLines(void) { return outLines; }

    enum { Type = 101 };
    int type(void) const Q_DECL_OVERRIDE;

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;

private:
    Mode mode;
    QRect rect;
    QBrush brush;
    QPen pen;
    QList<MTNodeLine *> inLines;
    QList<MTNodeLine *> outLines;
    MTNodeLine * newLine;
};

#endif // MTNODESOCKET_H
