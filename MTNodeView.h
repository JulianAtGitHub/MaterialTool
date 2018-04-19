#ifndef MTNODEVIEW_H
#define MTNODEVIEW_H

#include <QGraphicsView>

class MTNodeView : public QGraphicsView {
    Q_OBJECT

public:
    explicit MTNodeView(QWidget *parent = Q_NULLPTR);
    virtual ~MTNodeView(void);

    void initSceneUi(void);

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

private:
    bool isDragging;
    QPoint prevPos;
};

#endif // MTNODEVIEW_H
