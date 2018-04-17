#include "ui_MainWindow.h"
#include "MTWindow.h"

MTWindow::MTWindow(QWidget *parent)
: QMainWindow(parent)
{
    _ui = new Ui::mainWindow;
    _ui->setupUi(this);

    _ui->nodeView->initSceneUi();
}

MTWindow::~MTWindow(void) {

}
