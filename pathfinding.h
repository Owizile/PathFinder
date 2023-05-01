
#ifndef PATHFINDING_H
#define PATHFINDING_H

#pragma once
#include "grid.h"
class PathFinding
{
public:
    PathFinding(Grid _grid);
    QList<Node> findPath();
private:
    Grid grid;
    void retracePath(Node startNode, Node finishNode);
    long double getDistance(Node nodeA, Node nodeB);
    int coeff(int diff);
};

#endif // PATHFINDING_H
