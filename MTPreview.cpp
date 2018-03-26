#include "MTPreview.h"

MTPreview::MTPreview(QWidget * parent) :
QOpenGLWidget(parent)
{

}

void MTPreview::initializeGL() {
    initializeOpenGLFunctions();
}

void MTPreview::resizeGL(int w, int h) {
    QOpenGLWidget::resizeGL(w, h);
}

void MTPreview::paintGL() {

}
