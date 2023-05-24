#include "grid.h"

void Grid::start(float _radius, QRectF sceneRect, MapScene* mapScene) {
    for(int x = 0; x < grid.length(); x++){
        grid.value(x).clear();
    }
    grid.clear();
    bool testStart(true), testFinish(true);
    nodeRadius = _radius;
    nodeDiameter = nodeRadius * 2;
    gridSizeX = qRound(sceneRect.width()/nodeDiameter);
    gridSizeY = qRound(sceneRect.height()/nodeDiameter);
    for(int x = 0; x < gridSizeX; x++){
        QVector<Node> tempVector;
        for(int y = 0; y < gridSizeY; y++){
            qreal worldPointX, worldPointY;
            worldPointX = sceneRect.left() + x*nodeDiameter;
            worldPointY = sceneRect.top() + y*nodeDiameter;
            int difficulty = mapScene->getDifficulty(worldPointX,worldPointY, nodeDiameter);
            bool walkable = true;
            if(difficulty == 100){
                walkable = false;
            }
            if(mapScene->gridStart(worldPointX,worldPointY,nodeDiameter) && testStart){
                gridStartX = x;
                gridStartY = y;
                testStart = false;
            }
            if(mapScene->gridFinish(worldPointX,worldPointY,nodeDiameter) && testFinish){
                gridFinishX = x;
                gridFinishY = y;
                testFinish = false;
            }

            Node tempNode(walkable,worldPointX,worldPointY,x,y, difficulty);
            tempVector.push_back(tempNode);
        }
        grid.push_back(tempVector);
    }
}

void Grid::drawPath(MapScene* mapScene) {
    mapScene->removePath();
    QColor color = Qt::darkMagenta;
    QPen pen(color, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    for(qsizetype i = 1; i < path.length(); i++){
        if(i > 1){
            QGraphicsLineItem* currentLine = new QGraphicsLineItem(path[i-1].worldPositionX+nodeRadius,
                                                                   path[i-1].worldPositionY+nodeRadius,
                                                                   path[i].worldPositionX+nodeRadius,
                                                                   path[i].worldPositionY+nodeRadius);
            currentLine->setPen(pen);
            mapScene->addPathItem(currentLine);
        }
    }
}

Node Grid::getNode(int x, int y) {
    return grid[x][y];
}

Node Grid::nodeByWorldPosition(qreal _worldPosX, qreal _worldPosY) {
    double relX = (_worldPosX + gridWorldSizeX/2)/gridWorldSizeX;
    double relY = (_worldPosY + gridWorldSizeY/2)/gridWorldSizeY;
    relX = std::clamp(relX, 0.0, 1.0);
    relY = std::clamp(relY, 0.0, 1.0);

    int x = qRound((gridWorldSizeX - 1) * relX);
    int y = qRound((gridWorldSizeY - 1) * relY);
    return grid[x][y];
}

QList<Node*> Grid::getNeighbours(Node node) {
    QList<Node*> neighbours;
    for(int x = -1; x <= 1; x++){
        for(int y = -1; y <=1; y++){
            if(x == 0 && y == 0){
                continue;
            }
            int checkX = node.gridX+x;
            int checkY = node.gridY+y;
            if(checkX >= 0 && checkX < gridSizeX && checkY >= 0 && checkY < gridSizeY){
                neighbours.append(&grid[checkX][checkY]);
            }
        }
    }
    return neighbours;
}

int Grid::maxSize() {
    return gridSizeX*gridSizeY;
}
