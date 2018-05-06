#ifndef MTWINDOW_H
#define MTWINDOW_H

#include <QMainWindow>

class QFrame;
class QGraphicsScene;

class MTNodeView;
class MTSideBar;

namespace Ui {
    class mainWindow;
}

class MTWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MTWindow(QWidget *parent = Q_NULLPTR);
    virtual ~MTWindow(void);

    const MTNodeView * nodeView(void) const;

private:
    Ui::mainWindow *ui;
};

#endif // MTWINDOW_H
