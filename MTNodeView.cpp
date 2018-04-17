#include <QGraphicsScene>
#include "MTNodeView.h"

MTNodeView::MTNodeView(QWidget *parent)
: QGraphicsView(parent)
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
