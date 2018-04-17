#ifndef MTNODEVIEW_H
#define MTNODEVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>

class MTNodeView : public QGraphicsView {
    Q_OBJECT

public:
    explicit MTNodeView(QWidget *parent = Q_NULLPTR);
    virtual ~MTNodeView(void);

    void initSceneUi(void);
};

#endif // MTNODEVIEW_H
