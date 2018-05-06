#include <QVBoxLayout>
#include "MTSideBar.h"
#include "MTNodeItem.h"
#include "MTNodeView.h"
#include "MTWindow.h"

MTSideBar::MTSideBar(QWidget* parent)
: QFrame(parent)
{

}

MTSideBar::~MTSideBar(void) {

}

void MTSideBar::onAddBoxClicked(void) {
    MTWindow *window = qobject_cast<MTWindow *>(this->window());
    MTNodeItem *box = new MTNodeItem();
    QGraphicsScene *scene = window->nodeView()->scene();
    scene->addItem(box);
    box->setPos(scene->width() / 2.0, scene->height() / 2.0);
}

