#include "MTFlowview.h"

MTFlowview::MTFlowview(QWidget * parent) :
QOpenGLWidget(parent)
{

}

void MTFlowview::initializeGL() {
    initializeOpenGLFunctions();

    glClearColor(0.1804f, 0.1843f, 0.1882f, 1.0f);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

void MTFlowview::resizeGL(int w, int h) {
    QOpenGLWidget::resizeGL(w, h);
}

void MTFlowview::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

