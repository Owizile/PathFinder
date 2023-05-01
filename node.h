
#ifndef NODE_H
#define NODE_H
#pragma once
#include "mapeditor.h"



class Node
{
public:
    bool walkable;
    int difficulty;
    long double gCost, hCost;
    int gridX, gridY;
    qreal worldPositionX, worldPositionY;
    Node(bool _walkable = false, qreal _worldPosX = 0, qreal _worldPosY = 0, int _gridX = 0, int _gridY = 0, int _difficulty = 0);
    Node value();
    long double fCost();
    bool operator== (Node node);
    bool operator!= (Node node);
    bool operator >(Node node);
    int parentX, parentY;
};

#endif // NODE_H
