#include "mapscene.h"

void MapScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    switch(sceneMode){
    case DrawMode:
        if (event->button() == Qt::LeftButton){
            if (polygonItem == nullptr) {
                // create a new polygon item and start a new line
                QPointF point = event->scenePos();
                currentPolygon << point;
                polygonItem = addPolygon(currentPolygon, QPen(Qt::black), QBrush(Qt::gray));
                currentLineItem = addLine(point.x(), point.y(), point.x(), point.y(), QPen(Qt::black));
            }
            else {
                // add a new point to the polygon and update the current line
                QPointF point = event->scenePos();
                currentPolygon << point;
                QLineF line = QLineF(currentPolygon.last(), point);
                currentLineItem = addLine(line, QPen(Qt::black));
            }
        }
        break;
    case EditMode:
        QGraphicsScene::mousePressEvent(event);
        break;
    case StartMarkMode:
        if (event->button() == Qt::LeftButton) {
            if (startMarkItem == nullptr) {
                QPixmap pixmap(":/svg/resources/startMarker.svg");
                startMarkItem = new QGraphicsPixmapItem(pixmap);
                startMarkItem->setScale(1.5);
                addItem(startMarkItem);
            }
            QRectF boundingRect = startMarkItem->boundingRect();
            QPointF pos(event->scenePos().x() - boundingRect.width() / 2.0,
                        event->scenePos().y() - boundingRect.height());
            startMarkItem->setPos(pos);
        }
        break;
    case FinishMarkMode:
        if (event->button() == Qt::LeftButton) {
            if (finishMarkItem == nullptr) {
                QPixmap pixmap(":/svg/resources/finishMarker.svg");
                finishMarkItem = new QGraphicsPixmapItem(pixmap);
                finishMarkItem->setScale(1.5);
                addItem(finishMarkItem);
            }
            QRectF boundingRect = finishMarkItem->boundingRect();
            QPointF pos(event->scenePos().x() - boundingRect.width() / 2.0,
                        event->scenePos().y() - boundingRect.height());
            finishMarkItem->setPos(pos);
        }
        break;
    }
}

void MapScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    switch(sceneMode){
    case DrawMode:
        if (currentLineItem != nullptr) {
            // update the current line with the mouse position
            QPointF point = event->scenePos();
            QLineF line = QLineF(currentPolygon.last(), point);
            currentLineItem->setLine(line);
        }
        break;
    case EditMode:
        QGraphicsScene::mouseMoveEvent(event);
        break;
    case FinishMarkMode:
        break;
    case StartMarkMode:
        break;
    }
}

void MapScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    switch(sceneMode){
    case DrawMode:
        if (polygonItem != nullptr) {
            // finish the polygon and remove the current line
            QPointF point = event->scenePos();
            currentPolygon << point;
            polygonItem->setPolygon(currentPolygon);
            removeItem(currentLineItem);
            currentLineItem = nullptr;
        }
        break;
    case EditMode:
        QGraphicsScene::mouseReleaseEvent(event);
        break;
    case FinishMarkMode:
        break;
    case StartMarkMode:
        break;
    }
}

int MapScene::getMode() {
    return sceneMode;
}

void MapScene::setMode(int mode) {
    sceneMode = mode;
}

void MapScene::toggleMode(int mode) {
    switch(mode) {
    case EditMode:
        if (sceneMode == EditMode) {
            setMode(DrawMode);
            popUp->setPopupText("Draw mode setted");
        }
        else {
            setMode(EditMode);
            popUp->setPopupText("Edit mode setted");
        }
        popUp->show();
        break;
    case StartMarkMode:
        if (sceneMode == StartMarkMode) {
            setMode(DrawMode);
            popUp->setPopupText("Draw mode setted");
        }
        else {
            delete startMarkItem;
            startMarkItem = nullptr;
            setMode(StartMarkMode);
            popUp->setPopupText("Start mark can be placed");
        }
        popUp->show();
        break;
    case FinishMarkMode:
        if (sceneMode == FinishMarkMode) {
            setMode(DrawMode);
            popUp->setPopupText("Draw mode setted");
        }
        else {
            delete finishMarkItem;
            finishMarkItem = nullptr;
            setMode(FinishMarkMode);
            popUp->setPopupText("Finish mark can be placed");
        }
        popUp->show();
        break;
    }
}

QPointF polygonCenter(QPolygonF polygon) {
    QPointF center;
    center.setX(0);
    center.setY(0);

    double partialSignedArea = 0.0;
    double signedArea = 0.0;
    double x0 = 0.0; // Current vertex X
    double y0 = 0.0; // Current vertex Y
    double x1 = 0.0; // Next vertex X
    double y1 = 0.0; // Next vertex Y
    qsizetype vertexCount = polygon.size();

    for (int i = 0; i < vertexCount; i++) {
        x0 = polygon[i].x();
        y0 = polygon[i].y();
        x1 = polygon[(i+1) % vertexCount].x();
        y1 = polygon[(i+1) % vertexCount].y();
        partialSignedArea = x0*y1 - x1*y0;
        signedArea += partialSignedArea;
        center.setX(((x0 + x1)* partialSignedArea) + center.x());
        center.setY(((y0 + y1)* partialSignedArea) + center.y());
    }
    signedArea *= 0.5;
    center.setX(center.x() / (6.0*signedArea));
    center.setY(center.y() / (6.0*signedArea));
    return center;
}

bool isSelfIntersecting(const QPolygonF& polygon) {
    for (int i = 0; i < polygon.size() - 1; i++) {
        const QPointF& p1 = polygon.at(i);
        const QPointF& p2 = polygon.at(i+1);

        for (int j = i+2; j < polygon.size(); j++) {
            const QPointF& q1 = polygon.at(j);
            const QPointF& q2 = polygon.at((j+1) % polygon.size());

            QLineF edge1(p1, p2);
            QLineF edge2(q1, q2);

            if (edge1.intersects(edge2) == 1) {
                if (edge1.p1() == edge2.p1() || edge1.p1() == edge2.p2() ||
                    edge1.p2() == edge2.p1() || edge1.p2() == edge2.p2()) {
                    continue;
                }  // skip intersection if edges share a vertex

                return true;
            }
        }
    }

    return false;
}

void MapScene::addCurrentPolygon() {
    if (currentPolygon.empty()){
        return;
    }

    if (isSelfIntersecting(currentPolygon)){
        deleteCurrentPolygon();
        popUp->setPopupText("Polygon isn't simple");
        popUp->show();
        return;
    }

    // Check if the new polygon intersects with any of the existing polygons
    for (const QGraphicsPolygonItem* existingPolygon : polygonItems) {
        if (currentPolygon.intersects(existingPolygon->polygon())) {
            // If there is an intersection, remove the current polygon and return
            deleteCurrentPolygon();
            popUp->setPopupText("Polygon intersects with another one");
            popUp->show();
            return;
        }
    }

    // create a QSpinBox and set its position to the center of the circumscribed circle
    QSpinBox* spinBox = new QSpinBox();
    spinBox->setStyleSheet("QSpinBox {"
                               "font-family: Times New Roman;"
                               "font-size: 20px;"
                               "border: none;"
                               "background-color: rgba(0,0,0,0);"
                               "selection-background-color: rgba(0,0,0,0);"
                               "selection-color: rgb(0,0,0);"

                           "}"
                           "QSpinBox::up-button, QSpinBox::down-button {"
                               "width: 0px;"
                               "height: 0px;"
                           "}");
    spinBox->setSuffix("%");
    spinBox->setRange(0, 100);
    spinBox->setValue(100);

    QPointF center = polygonCenter(currentPolygon);

    QGraphicsProxyWidget* spinBoxProxy = addWidget(spinBox);
    spinBoxProxy->setPos(center.x() - spinBoxProxy->boundingRect().width() / 2,
                          center.y() - spinBoxProxy->boundingRect().height() / 2);

    // If there is no intersection, add the new polygon to the list and clear the current polygon
    polygonItems << polygonItem;
    spinBoxes << spinBoxProxy;

    connect(spinBox, &QSpinBox::valueChanged, [=](int value) {
        QColor color = Qt::gray;
        color.setAlphaF(value / 100.0);
        int index;
        for (int i = 0; i < spinBoxes.size(); i++) {
            if (spinBoxes[i]->widget() == spinBox) {
                index = i;
            }
        }
        polygonItems[index]->setBrush(QBrush(color));
    });

    currentPolygon = QPolygonF();
    polygonItem = nullptr;
    currentLineItem = nullptr;
}

void MapScene::deleteCurrentPolygon() {
    if (polygonItem == nullptr && !polygonItems.empty()) {
        QGraphicsPolygonItem* item = polygonItems.takeLast();
        delete item;
        QGraphicsProxyWidget* widget = spinBoxes.takeLast();
        delete widget;
        return;
    }
    else if (polygonItem == nullptr) {
        return;
    }
    removeItem(polygonItem);
    polygonItem = nullptr;
    currentLineItem = nullptr;
    currentPolygon = QPolygonF();
}

QPointF MapScene::getStartMarkPos() {
    QPointF startMarkPos(startMarkItem->pos().x(),
                         startMarkItem->pos().y());
    return startMarkPos;
}

QPointF MapScene::getFinishMarkPos() {
    QPointF finishMarkPos(finishMarkItem->pos().x(),
                          finishMarkItem->pos().y());
    return finishMarkPos;
}

void MapScene::clear() {
    QGraphicsScene::clear();
    polygonItems = QList<QGraphicsPolygonItem*>();
    spinBoxes = QList<QGraphicsProxyWidget*>();
    currentPolygon = QPolygonF();
    startMarkItem = nullptr;
    finishMarkItem = nullptr;
    polygonItem = nullptr;
    currentLineItem = nullptr;
}
