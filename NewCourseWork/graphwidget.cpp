/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <math.h>
#include <QKeyEvent>
#include <QRandomGenerator>

#include "graphwidget.h"
#include "edge.h"
#include "node.h"

//! [0]
GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent), timerId(0)
{
    myscene = new QGraphicsScene(this);
    myscene->setItemIndexMethod(QGraphicsScene::NoIndex);
    myscene->setSceneRect(-2500, -2500, 5000, 5000);
    setScene(myscene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(500, 500);
    nodes = new QVector<Vertex*>;
    //! [0]

    //! [1]
}

void GraphWidget::draw(Node *node1, Node *node2) {
    if (!nodes->contains(node1)) {
        myscene->addItem(node1);
        node1->setPos(rand()%100, rand()%100);
        node1->setZValue(2);
        nodes->append(node1);
    }
    if (!nodes->contains(node2)) {
        myscene->addItem(node2);
        node2->setPos(rand()%100, rand()%100);
        node2->setZValue(2);
        nodes->append(node2);
    }
    myscene->addItem(new Edge(node1, node2));
}

void GraphWidget::setRadius(int r) {
    R = r + 16;
}

void GraphWidget::setCheck(bool c) {
    center = c;
}

void GraphWidget::setCurrentVertex(int text) {
    int i = 0;
    while(nodes->at(i)->text != QString::number(text))
        i++;
    currentVertex = nodes->at(i);
}

void GraphWidget::draw(int text, int front, int count) {
    Vertex *vertex = new Vertex(this);
    nodes->append(vertex);
    if  (!center) {
        center = true;
        myscene->addItem(vertex);
        vertex->setPos(0, 0);
        vertex->setZValue(1);
        vertex->addText(text);
        return;
    }
    static double x;
    static int current = -1;
    if (front != current) {
        current = front;
        R += front*count*2;
        R += 16;
        myscene->addEllipse(-R, -R, R*2, R*2, QPen(Qt::gray, 0));
        x = 0;
    }
    myscene->addItem(vertex);
    vertex->addText(text);
    vertex->setZValue(1);

    vertex->setPos(R * cos(x), R * sin(x));
    x += 3.14*2/count;
    myscene->update();
}

void GraphWidget::drawPath(int text) {
    int i = 0;
    while(nodes->at(i)->text != QString::number(text+1))
        i++;
    myscene->addItem(new Edge(currentVertex, nodes->at(i)));
    currentVertex = nodes->at(i);
    myscene->update();
}

//! [2]
void GraphWidget::itemMoved()
{
    if (!timerId)
        timerId = startTimer(1000 / 25);
}
//! [2]

//! [3]
void GraphWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    case Qt::Key_Space:
    case Qt::Key_Enter:
        shuffle();
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}
//! [3]

//! [4]
void GraphWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    QList<Node *> nodes;
    foreach (QGraphicsItem *item, scene()->items()) {
        if (Node *node = qgraphicsitem_cast<Node *>(item))
            nodes << node;
    }

    foreach (Node *node, nodes)
        node->calculateForces();

    bool itemsMoved = false;
    foreach (Node *node, nodes) {
        if (node->advance())
            itemsMoved = true;
    }

    if (!itemsMoved) {
        killTimer(timerId);
        timerId = 0;
    }
}
//! [4]

#if QT_CONFIG(wheelevent)
//! [5]
void GraphWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
//! [5]
#endif

//! [7]
void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}
//! [7]

void GraphWidget::shuffle()
{
    foreach (QGraphicsItem *item, scene()->items()) {
        if (qgraphicsitem_cast<Node *>(item))
            item->setPos(-150 + QRandomGenerator::global()->bounded(300), -150 + QRandomGenerator::global()->bounded(300));
    }
}

void GraphWidget::zoomIn()
{
    scaleView(qreal(1.2));
}

void GraphWidget::zoomOut()
{
    scaleView(1 / qreal(1.2));
}
