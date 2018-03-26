#ifndef MTWINDOW_H
#define MTWINDOW_H

#include <QMainWindow>

namespace Ui {
class MTWindow;
}

class MTWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MTWindow(QWidget *parent = 0);
    ~MTWindow();

private:
    Ui::MTWindow *ui;
};

#endif // MTWINDOW_H
