#include "pathfinding.h"

PathFinding::PathFinding(Grid _grid) {
    grid = _grid;
}

QList<Node> PathFinding::findPath() {
    Node startNode = grid.getNode(grid.gridStartX,grid.gridStartY);
    Node finishNode = grid.getNode(grid.gridFinishX,grid.gridFinishY);

    QList<Node> openSet = QList<Node>();
    QList<Node> closedSet = QList<Node>();
    openSet.append(startNode);
    openSet[0].gCost = 0;
    openSet[0].hCost = getDistance(startNode,finishNode);
    while(openSet.length()>0){
        Node currNode = openSet[0];
        for(qsizetype i = 1; i < openSet.length(); i++){
            if ((openSet[i].fCost() < currNode.fCost()) ||
               ((openSet[i].fCost() == currNode.fCost()) && (openSet[i].hCost < currNode.hCost))){
                currNode = openSet[i];
            }
        }
        for(qsizetype i = 0; i < openSet.length(); i++){
            if((openSet[i].gridX == currNode.gridX) && (openSet[i].gridY == currNode.gridY)){
                openSet.remove(i);
            }
        }
        closedSet.append(currNode);
        if ((currNode.gridX == finishNode.gridX) && (currNode.gridY == finishNode.gridY)){
            retracePath(startNode,finishNode);
            return grid.path;
        }
        QList<Node*> neighbours = grid.getNeighbours(currNode);
        for(qsizetype i = 0; i < neighbours.length(); i++){
            Node* neighbour = (neighbours[i]);
            bool contain = false;
            for(qsizetype i = 0; i < closedSet.length() ; i++){
                if((closedSet[i].gridX == neighbour->gridX) && (closedSet[i].gridY == neighbour->gridY)){
                    contain = true;
                }
            }
            if(!neighbour->walkable || contain){
                continue;
            }
            finishNode.parentX = currNode.gridX;
            finishNode.parentY = currNode.gridY;
            long double newMoveCostToNeighbour = currNode.gCost + getDistance(currNode, *neighbour)
                                                                + coeff(currNode.difficulty) * getDistance(currNode, *neighbour);
            if((newMoveCostToNeighbour < neighbour->gCost) && (!contain)){
                neighbour->gCost = newMoveCostToNeighbour;
                neighbour->hCost = getDistance(*neighbour, finishNode);
                neighbour->parentX = currNode.gridX;
                neighbour->parentY = currNode.gridY;
                if(!contain){
                    openSet.append(*neighbour);
                }
            }
        }
    }
    return grid.path;
}

float PathFinding::getDistance(Node nodeA, Node nodeB) {
    int dstX = std::abs(nodeA.gridX - nodeB.gridX);
    int dstY = std::abs(nodeA.gridY - nodeB.gridY);
    return (qMin(dstX,dstY)*14 + 10*(qMax(dstX,dstY)-qMin(dstX,dstY)));
}

void PathFinding::retracePath(Node startNode, Node finishNode) {
    QList<Node> path = QList<Node>();
    Node currNode = finishNode;
    while((currNode.gridX != startNode.gridX)||(currNode.gridY != startNode.gridY)){
        path.append(currNode);
        currNode = grid.getNode(currNode.parentX,currNode.parentY);
    }
    grid.path = path;
}

float PathFinding::coeff(int diff) {
    return diff / 100.0;
}
