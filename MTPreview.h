#ifndef MTPREVIEW_H
#define MTPREVIEW_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>

class MTPreview : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {

public:
    MTPreview(QWidget * parent);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
};

#endif // MTPREVIEW_H
