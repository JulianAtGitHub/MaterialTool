#ifndef MTNODEITEM_H
#define MTNODEITEM_H

#include <QGraphicsItem>
#include <QRect>
#include <QBrush>
#include <QPen>

class MTNodeSocket;

class MTNodeItem: public QGraphicsItem {

public:
    explicit MTNodeItem(QGraphicsItem *parent = Q_NULLPTR);
    virtual ~MTNodeItem(void);

    virtual QPainterPath shape(void) const Q_DECL_OVERRIDE;
    virtual QRectF boundingRect(void) const Q_DECL_OVERRIDE;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) Q_DECL_OVERRIDE;

    inline MTNodeSocket & getInput(void) const { return *input; }
    inline MTNodeSocket & getOutput(void) const { return *output; }

    enum { Type = 100 };
    int type(void) const Q_DECL_OVERRIDE;

protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;

private:
    QRect rect;
    QBrush brush;
    QPen pen;
    QPen selPen;

    MTNodeSocket *input;
    MTNodeSocket *output;
};

#endif // MTNODEITEM_H
