#include "MTWindow.h"
#include "ui_MTWindow.h"

MTWindow::MTWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MTWindow)
{
    ui->setupUi(this);
}

MTWindow::~MTWindow() {
    delete ui;
}
