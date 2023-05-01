
#include "node.h"

Node::Node(bool _walkable, qreal _worldPosX, qreal _worldPosY,int _gridX, int _gridY, int _difficulty)
{
    walkable = _walkable;
    worldPositionX = _worldPosX;
    worldPositionY = _worldPosY;
    gridX = _gridX;
    gridY = _gridY;
    gCost = INT_MAX;
    hCost = INT_MAX;
    parentX = 0;
    parentY = 0;
    difficulty = _difficulty;

}
Node Node::value(){
    Node tempNode(walkable,worldPositionX,worldPositionY,gridX,gridY);
    return tempNode;
}
long double Node::fCost(){
    return (gCost + hCost);
}
bool Node::operator == (Node node){
    if((gridX == node.gridX) && (gridY == node.gridY)) return true;
    return false;
}
bool Node::operator!= (Node node){
    if((gridX == node.gridX) && (gridY == node.gridY)) return false;
    return true;
}
bool Node::operator >(Node node){
    if (fCost() > node.fCost()) return true;
    return false;
}
