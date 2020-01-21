#ifndef VERTEX_H
#define VERTEX_H

#include <QGraphicsItem>

#include "graphwidget.h"

class Vertex : public QGraphicsItem
{
public:
    Vertex(GraphWidget *graphWidget);
    void addText(int x);
    void setColor(QString c);

    QString text;
    QColor color = Qt::white;
    GraphWidget *graph;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // VERTEX_H
