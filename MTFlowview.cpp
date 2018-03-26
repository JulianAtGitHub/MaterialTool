#include "MTFlowview.h"

MTFlowview::MTFlowview(QWidget * parent) :
QOpenGLWidget(parent)
{

}

void MTFlowview::initializeGL() {
    initializeOpenGLFunctions();
}

void MTFlowview::resizeGL(int w, int h) {
    QOpenGLWidget::resizeGL(w, h);
}

void MTFlowview::paintGL() {

}

