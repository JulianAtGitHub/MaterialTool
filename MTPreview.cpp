#include "MTPreview.h"

MTPreview::MTPreview(QWidget * parent) :
QOpenGLWidget(parent)
{

}

void MTPreview::initializeGL() {
    initializeOpenGLFunctions();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

void MTPreview::resizeGL(int w, int h) {
    QOpenGLWidget::resizeGL(w, h);
}

void MTPreview::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
