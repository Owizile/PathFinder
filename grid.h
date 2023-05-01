
#ifndef GRID_H
#define GRID_H

#pragma once
#include "mapeditor.h"
#include "node.h"
#include "mapscene.h"


class Grid
{
public:
    int worldSizeX, worldSizeY;
    int gridWorldSizeX, gridWorldSizeY;
    int gridStartX, gridStartY;
    int gridFinishX, gridFinishY;
    int maxSize();
    qreal nodeRadius;
    void start(float _radius, QRectF sceneRect, MapScene* mapScene);
    Node nodeByWorldPosition(qreal _worldPosX, qreal _worldPosY);
    Node getNode(int x, int y);
    QList<Node*> getNeighbours(Node node);
    QList<Node> path;
    void drawPath(MapScene* mapScene);
private:
    QList<QList<Node>> grid;
    qreal nodeDiameter;
    int gridSizeX, gridSizeY;
};

#endif // GRID_H
