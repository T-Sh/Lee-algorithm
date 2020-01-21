#include <QPainter>
#include <QStyleOption>
#include <QGraphicsScene>

#include "vertex.h"

Vertex::Vertex(GraphWidget *graphWidget)
    : graph(graphWidget)
{
    setCacheMode(DeviceCoordinateCache);
}

QRectF Vertex::boundingRect() const
{
    qreal adjust = 2;
    return QRectF( -10 - adjust, -10 - adjust, 23 + adjust, 23 + adjust);
}

void Vertex::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    QRadialGradient gradient(-3, -3, 10);
    if (option->state & QStyle::State_Sunken) {
        gradient.setCenter(3, 3);
        gradient.setFocalPoint(3, 3);
        gradient.setColorAt(1, color.light(120));
        gradient.setColorAt(0, QColor(Qt::gray).light(120));
    } else {
        gradient.setColorAt(0, Qt::white);
        gradient.setColorAt(1, color);
    }
    painter->setBrush(gradient);

    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(-10, -10, 20, 20);
    QFont font = painter->font();
    font.setBold(false);
    font.setPointSize(5);
    painter->setFont(font);
    painter->drawText(-2, 2, text);
}

void Vertex::addText(int x) {
    text = QString::number(x);
}

void Vertex::setColor(QString c) {
    color = QColor(c);
    update();
}
