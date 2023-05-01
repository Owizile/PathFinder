#ifndef MAPSCENE_H
#define MAPSCENE_H

#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QGraphicsSceneMouseEvent>
#include <QPolygonF>
#include <QGraphicsProxyWidget>
#include <QSpinBox>
#include <QObject>
#include "popup.h"

enum modes {DrawMode, EditMode, StartMarkMode, FinishMarkMode};

class MapScene : public QGraphicsScene
{
public:
    MapScene(QObject* parent = nullptr) : QGraphicsScene(parent) {};
    void clear();
    void addCurrentPolygon();
    void deleteCurrentPolygon();
    void setMode(int mode);
    void toggleMode(int mode);
    int getMode();
    QPointF getStartMarkPos();
    QPointF getFinishMarkPos();
    int getDifficulty(qreal worldPositionX, qreal worldPositionY,qreal diameter);
    bool gridStart(qreal worldPositionX, qreal worldPositionY,qreal diameter);
    bool gridFinish(qreal worldPositionX, qreal worldPositionY,qreal diameter);
    QGraphicsPixmapItem* startItem();
    QGraphicsPixmapItem* finishItem();
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
private:
    QList<QGraphicsPolygonItem*> polygonItems;
    QList<QGraphicsProxyWidget*> spinBoxes;
    QPolygonF currentPolygon;
    QGraphicsLineItem* currentLineItem = nullptr;
    QGraphicsPolygonItem* polygonItem = nullptr;
    QGraphicsPixmapItem* startMarkItem = nullptr;
    QGraphicsPixmapItem* finishMarkItem = nullptr;
    int sceneMode = DrawMode;
    PopUp *popUp = new PopUp();
};

#endif // MAPSCENE_H
