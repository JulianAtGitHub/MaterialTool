#ifndef MTNODELINE_H
#define MTNODELINE_H

#include <QGraphicsPathItem>
#include <QPen>

class MTNodeSocket;

class MTNodeLine : public QGraphicsPathItem {

public:
    explicit MTNodeLine(const QPointF &a, const QPointF &b, QGraphicsItem *parent = Q_NULLPTR);
    virtual ~MTNodeLine(void);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) Q_DECL_OVERRIDE;

    inline void setPointA(const QPointF &point) { pointA = point; updatePath(); }
    inline const QPointF & getPointA(void) const { return pointA; }

    inline void setPointB(const QPointF &point) { pointB = point; updatePath(); }
    inline const QPointF & getPointB(void) const { return pointB; }

    inline void setSource(MTNodeSocket *item) { source = item; }
    inline MTNodeSocket * getSource(void) const { return source; }

    inline void setTarget(MTNodeSocket *item) { target = item; }
    inline MTNodeSocket * getTarget(void) const { return target; }

    enum { Type = 102 };
    int type(void) const Q_DECL_OVERRIDE;

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;

private:
    void updatePath(void);

    QPen pen;
    QPointF pointA;
    QPointF pointB;
    MTNodeSocket *source;
    MTNodeSocket *target;

};

#endif // MTNODELINE_H
