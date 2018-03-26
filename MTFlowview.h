#ifndef MTFLOWVIEW_H
#define MTFLOWVIEW_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>

class MTFlowview : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {

public:
    MTFlowview(QWidget * parent);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
};

#endif // MTFLOWVIEW_H
