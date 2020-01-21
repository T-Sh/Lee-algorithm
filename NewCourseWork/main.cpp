#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Волновой алгоритм");
    w.show();
    srand(time(NULL));

    return a.exec();
}
