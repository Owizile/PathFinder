#ifndef PATHFINDING_H
#define PATHFINDING_H
#include "grid.h"

class PathFinding {
public:
    PathFinding(Grid _grid);
    QList<Node> findPath();
private:
    Grid grid;
    void retracePath(Node startNode, Node finishNode);
    float getDistance(Node nodeA, Node nodeB);
    float coeff(int diff);
};

#endif // PATHFINDING_H
