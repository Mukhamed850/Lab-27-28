#include "MyTxtEditor.h"
#include <QApplication>
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MyTxtEditor w;
    w.show();
    return a.exec();
}
