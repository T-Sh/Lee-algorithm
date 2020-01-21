#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    vertexes = new QVector<Node*>;

    ui->setupUi(this);

    //Главный виджет для размещения
    QHBoxLayout *main = new QHBoxLayout();

    //Виджет для SVG
    QVBoxLayout *left = new QVBoxLayout();
    circle = new GraphWidget;
    left->addWidget(circle);

    //Управляющее поле
    QVBoxLayout *right = new QVBoxLayout();
    QPushButton *load = new QPushButton("Загрузить");
    count = new QLineEdit();
    QPushButton *generate = new QPushButton("Сгенерировать");
    QPushButton *algorithm = new QPushButton("Волновой алгоритм");
    QPushButton *byStep = new QPushButton("Алгоритм по шагам");
    graph = new GraphWidget;

    count->setValidator( new QIntValidator(6, 40, this));
    count->setPlaceholderText("Введите количество вершин");

    right->addWidget(load);
    right->addWidget(count);
    right->addWidget(generate);
    right->addWidget(algorithm);
    right->addWidget(byStep);
    right->addWidget(graph);

    main->addLayout(left);
    main->addLayout(right);

    ui->centralWidget->setLayout(main);

    QObject::connect(load, SIGNAL(clicked()), this, SLOT(load_clicked()));
    QObject::connect(generate, SIGNAL(clicked()), this, SLOT(gen_clicked()));
    QObject::connect(algorithm, SIGNAL(clicked()), this, SLOT(alg_clicked()));
    QObject::connect(byStep, SIGNAL(clicked()), this, SLOT(byStep_clicked()));
}

void MainWindow::createGraph() {
    if (vertexes->isEmpty()) return;
    graph->scene()->clear();
    graph->nodes->clear();
    circle->scene()->clear();
    circle->nodes->clear();

    start = -1;
    finish = -1;

    for (int i = 0; i < vertexes->size(); i++) {
        for (int j = i+1; j < vertexes->size(); j++) {
            if (vertexes->at(i)->nodes->at(j) == "1") {
                vertexes->at(i)->addText(i+1);
                vertexes->at(j)->addText(j+1);
                graph->draw(vertexes->at(i), vertexes->at(j));
            }
        }
    }
}

void MainWindow::load_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), "",  tr("*.txt"));
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);

    vertexes->clear();

    QString str;
    int i = 0;
    while(!file.atEnd()) {
        QVector<QString> *vector = new QVector<QString>;
        str = file.readLine();
        str.remove("\r\n");
        *vector = str.split(' ').toVector();
        i++;
        Node *node = new Node(graph);
        node->nodes = vector;
        vertexes->append(node);
    }
    file.close();
    createGraph();
}

void MainWindow::gen_clicked() {
    vertexes->clear();
    start = -1;
    finish = -1;
    int c = this->count->text().toInt();

    for (int i = 0; i < c; i++) {
        QVector<QString> *vector = new QVector<QString>;
        for (int j = 0; j < c; j++) {
            if (j < i) vector->append(vertexes->at(j)->nodes->at(i));
            if (j == i) vector->append("0");
            if (j > i) vector->append(QString::number((rand()%10) % 2));
        }

        Node *node = new Node(graph);
        node->nodes = vector;
        vertexes->append(node);
    }

    createGraph();
}

void MainWindow::alg_clicked() {
    if (!alg()) return;
    findPath();
    drawVertex();
    drawPath();
}

bool MainWindow::alg() {
    check();

    if (start < 0 || finish < 0) return false;

    step = 0;
    vertexes->at(start)->setFront(step);

    while (true) {
        for (int i = 0; i < vertexes->size(); i++) {
            if (vertexes->at(i)->getFront() == step) {
                for (int j = 0; j < vertexes->at(i)->nodes->size(); j++) {
                    if (vertexes->at(i)->nodes->at(j) == "1" && vertexes->at(j)->getFront() < 0) {
                        vertexes->at(j)->setFront(step+1);
                    }
                }
            }
        }
        if  (vertexes->at(finish)->getFront() > 0) break;
        step++;
    }
    step++;
    return true;
}

void MainWindow::drawVertex() {
    int tempStep = 0;
    currentStep = 0;
    circle->draw(start+1, tempStep, 1);
    tempStep = 1;

    circle->setRadius(15*tempStep);

    while (tempStep <= step) {
        QVector<int> temp;
        for (int i = 0; i < vertexes->size(); i++) {
            if (vertexes->at(i)->getFront() == tempStep) {
                if (vertexes->at(i)->color == Qt::yellow || i == start || i == finish)
                    temp.push_front(i);
                else
                    temp.append(i);
            }
        }

        for(int i = 0; i < temp.size(); i++) {
            circle->draw(temp.at(i)+1, tempStep, temp.size());
        }
        tempStep++;
    }
}

void MainWindow::findPath() {
    int tempStep = step;
    int current = finish;

    while(tempStep > 1) {
        for (int i = 0; i < vertexes->at(current)->nodes->size(); i++) {
            if (vertexes->at(i)->getFront() == tempStep-1 && vertexes->at(current)->nodes->at(i) == "1") {
                current = i;
                tempStep--;
                vertexes->at(current)->setColor("yellow");
                break;
            }
        }
    }
}

void MainWindow::drawPath() {
    circle->setCurrentVertex(finish+1);

    for (int i = 0; i < vertexes->size(); i++) {
        if (vertexes->at(i)->color == Qt::yellow)
            circle->drawPath(i);
    }

    circle->drawPath(start);
}

void MainWindow::byStep_clicked() {
    if (currentStep == 0) {
        if (!alg()) return;
        currentStep = 1;
        circle->draw(start+1, currentStep, 1);
        circle->setRadius(15*currentStep);
    }

    findPath();

    while (currentStep <= step) {
        QVector<int> temp;
        for (int i = 0; i < vertexes->size(); i++) {
            if (vertexes->at(i)->getFront() == currentStep) {
                if (vertexes->at(i)->color == Qt::yellow || i == start || i == finish)
                    temp.push_front(i);
                else
                    temp.append(i);
            }
        }

        for(int i = 0; i < temp.size(); i++) {
            circle->draw(temp.at(i)+1, currentStep, temp.size());
        }
        currentStep++;
        return;
    }
    currentStep = 0;
    drawPath();
}

void MainWindow::check() {
    for (QGraphicsItem *vertex : circle->scene()->items())
        circle->scene()->removeItem(vertex);
    circle->scene()->clear();
    circle->nodes->clear();
    circle->setRadius(0);
    circle->setCheck(false);
    start = -1;
    finish = -1;

    for (int i = 0; i < vertexes->size(); i++) {
        if (vertexes->at(i)->getColor() == QColor("red")) start = i;
        if (vertexes->at(i)->getColor() == QColor("blue")) finish = i;
        vertexes->at(i)->setFront(-1);
    }
}

MainWindow::~MainWindow()
{
    circle->scene()->clear();
    graph->scene()->clear();
    delete ui;
    delete circle;
    delete graph;
    delete count;
    delete vertexes;
}
