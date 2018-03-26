#include "MTWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MTWindow w;
    w.show();

    return a.exec();
}
