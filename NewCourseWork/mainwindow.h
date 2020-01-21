#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QVector>
#include <QTableView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIntValidator>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QLabel>
#include <QStringList>

#include "graphwidget.h"
#include "node.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QVector<Node*> *vertexes;
    int start = -1;
    int finish = -1;
    int currentStep = 0;

protected:
    GraphWidget *graph;
    GraphWidget *circle;

private slots:
    void load_clicked();
    void gen_clicked();
    void alg_clicked();
    void byStep_clicked();

private:
    Ui::MainWindow *ui;
    QLineEdit *count;
    int step;

    void createGraph();
    void drawVertex();
    void findPath();
    void drawPath();
    void check();
    bool alg();
};

#endif // MAINWINDOW_H
