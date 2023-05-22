#ifndef MAPSCENE_H
#define MAPSCENE_H

#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QGraphicsSceneMouseEvent>
#include <QPolygonF>
#include <QGraphicsProxyWidget>
#include <QXmlStreamWriter>
#include <QFile>
#include <QSpinBox>
#include <QObject>
#include "popup.h"

enum Modes {DrawMode, EditMode, StartMarkMode, FinishMarkMode};

class MapScene : public QGraphicsScene {
public:
    MapScene(QObject* parent = nullptr) : QGraphicsScene(parent) {};
    void clear();
    void addCurrentPolygon(int polygonDifficulty = 100);
    void deleteCurrentPolygon();
    void addPathItem(QGraphicsLineItem* item);
    void removePath();
    void toggleMode(int mode);
    QString getModeName();
    void getStartMarkPos(QPointF& startMarkPos);
    void getFinishMarkPos(QPointF& finishMarkPos);
    int getDifficulty(qreal worldPositionX, qreal worldPositionY,qreal diameter, bool centered);
    bool gridStart(qreal worldPositionX, qreal worldPositionY,qreal diameter);
    bool gridFinish(qreal worldPositionX, qreal worldPositionY,qreal diameter);
    QGraphicsPixmapItem* getStartItem();
    QGraphicsPixmapItem* getFinishItem();
    void saveMapSceneToXML(QFile& file);
    void loadMapSceneFromXML(QFile& file);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
private:
    void setMode(int mode);
    int getMode();
    bool isMarkPosValid(QPointF markPoint);
    void sceneAdjustCheck();
    QList<QGraphicsPolygonItem*> polygonItems;
    QList<QGraphicsProxyWidget*> spinBoxes;
    QList<QGraphicsLineItem*> pathItems;
    QPolygonF currentPolygon;
    QGraphicsLineItem* currentLineItem = nullptr;
    QGraphicsPolygonItem* polygonItem = nullptr;
    QGraphicsPixmapItem* startMarkItem = nullptr;
    QGraphicsPixmapItem* finishMarkItem = nullptr;
    QGraphicsRectItem* sceneRectItem = nullptr;
    int sceneMode = DrawMode;
    PopUp *popUp = new PopUp();
};

#endif // MAPSCENE_H
