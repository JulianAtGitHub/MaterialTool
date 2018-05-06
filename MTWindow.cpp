#include "ui_MainWindow.h"
#include "MTWindow.h"

MTWindow::MTWindow(QWidget *parent)
: QMainWindow(parent)
{
    ui = new Ui::mainWindow;
    ui->setupUi(this);

    ui->nodeView->initSceneUi();
}

MTWindow::~MTWindow(void) {

}

const MTNodeView * MTWindow::nodeView(void) const {
    return ui->nodeView;
}
