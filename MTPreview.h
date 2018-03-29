#ifndef MTPREVIEW_H
#define MTPREVIEW_H

#include <QOpenGLWidget>

class MTPreview : public QOpenGLWidget {

public:
    MTPreview(QWidget * parent);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
};

#endif // MTPREVIEW_H
