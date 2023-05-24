#include "mapscene.h"

bool MapScene::isMarkPosValid(QPointF markPoint) {
    QPolygonF polygon;
    float gridNodeRadius = 10;
    for (QGraphicsPolygonItem* polyItem : polygonItems) {
        polygon = polyItem->polygon();
        if (getDifficulty(markPoint.x(), markPoint.y(), gridNodeRadius * 2) != 0){
            return false;
        }
    }
    return true;
}

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
                removePath();
            }
            QRectF boundingRect = startMarkItem->boundingRect();
            QPointF pos(event->scenePos().x() - boundingRect.width() / 2.0,
                        event->scenePos().y() - boundingRect.height());
            if(!isMarkPosValid(pos)) {
                popUp->setPopupText("Метка не может быть поставлена на полигон");
                popUp->show();
                delete startMarkItem;
                startMarkItem = nullptr;
                removePath();
                return;
            }
            startMarkItem->setPos(pos);
            removePath();
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
            if(!isMarkPosValid(pos)) {
                popUp->setPopupText("Метка не может быть поставлена на полигон");
                popUp->show();
                delete finishMarkItem;
                finishMarkItem = nullptr;
                removePath();
                return;
            }
            finishMarkItem->setPos(pos);
            removePath();
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

QString MapScene::getModeName() {
    QString currentMode;

    switch(getMode()) {
    case 0: { currentMode = "Рисование"; } break;
    case 1: { currentMode = "Редактирование"; } break;
    case 2: { currentMode = "Метка старта"; } break;
    case 3: { currentMode = "Метка финиша"; } break;
    }

    return currentMode;
}

void MapScene::setMode(int mode) {
    sceneMode = mode;
}

void MapScene::toggleMode(int mode) {
    switch(mode) {
    case EditMode:
        if (sceneMode == EditMode) {
            setMode(DrawMode);
            popUp->setPopupText("Режим рисования установлен");
        }
        else {
            setMode(EditMode);
            popUp->setPopupText("Режим редактирования установлен");
        }
        popUp->show();
        break;
    case StartMarkMode:
        if (sceneMode == StartMarkMode) {
            setMode(DrawMode);
            popUp->setPopupText("Режим рисования установлен");
        }
        else {
            delete startMarkItem;
            startMarkItem = nullptr;
            removePath();
            setMode(StartMarkMode);
            popUp->setPopupText("Метка старта может быть установлена");
        }
        popUp->show();
        break;
    case FinishMarkMode:
        if (sceneMode == FinishMarkMode) {
            setMode(DrawMode);
            popUp->setPopupText("Режим рисования установлен");
        }
        else {
            delete finishMarkItem;
            finishMarkItem = nullptr;
            removePath();
            setMode(FinishMarkMode);
            popUp->setPopupText("Метка финиша может быть установлена");
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

void MapScene::sceneAdjustCheck()
{
    QRectF curSceneRect = sceneRect();

    QPointF topRight = curSceneRect.topRight();
    QPointF topLeft = curSceneRect.topLeft();
    QPointF bottomRight = curSceneRect.bottomRight();
    QPointF bottomLeft = curSceneRect.bottomLeft();

    for (const QPointF& point : currentPolygon) {
        // Calculate the distance between the point and each of the four sides of the scene rect
        qreal distanceToTop = abs((topRight.x() - topLeft.x()) * (topLeft.y() - point.y()) -
                              (topLeft.x() - point.x()) * (topRight.y() - topLeft.y())) /
                              QLineF(topLeft, topRight).length();

        qreal distanceToRight = abs((bottomRight.x() - topRight.x()) * (topRight.y() - point.y()) -
                                (topRight.x() - point.x()) * (bottomRight.y() - topRight.y())) /
                                QLineF(topRight, bottomRight).length();

        qreal distanceToBottom = abs((bottomLeft.x() - bottomRight.x()) * (bottomRight.y() - point.y()) -
                                 (bottomRight.x() - point.x()) * (bottomLeft.y() - bottomRight.y())) /
                                 QLineF(bottomRight, bottomLeft).length();

        qreal distanceToLeft = abs((topLeft.x() - bottomLeft.x()) * (bottomLeft.y() - point.y()) -
                               (bottomLeft.x() - point.x()) * (topLeft.y() - bottomLeft.y())) /
                               QLineF(bottomLeft, topLeft).length();

        qreal allowedDistance = 10;

        //TODO: adjust only a certain side of sceneRect if needed

        if (distanceToTop < allowedDistance || distanceToRight < allowedDistance ||
            distanceToBottom < allowedDistance || distanceToLeft < allowedDistance) {
            curSceneRect = curSceneRect.adjusted(-allowedDistance, -allowedDistance, allowedDistance, allowedDistance);
        }
    }
    delete sceneRectItem;
    sceneRectItem = new QGraphicsRectItem(curSceneRect);
    sceneRectItem->setPen(QPen(Qt::transparent));
    addItem(sceneRectItem);
}

void MapScene::addCurrentPolygon(int polygonDifficulty) {
    if (currentPolygon.empty()){
        return;
    }
    else if (currentPolygon.size() == 2){
        deleteCurrentPolygon();
        popUp->setPopupText("Полигон не может быть линией");
        popUp->show();
        return;
    }

    if (isSelfIntersecting(currentPolygon)){
        deleteCurrentPolygon();
        popUp->setPopupText("Полигон не простой");
        popUp->show();
        return;
    }

    // Check if the new polygon intersects with any of the existing polygons
    for (const QGraphicsPolygonItem* existingPolygon : polygonItems) {
        if (currentPolygon.intersects(existingPolygon->polygon())) {
            // If there is an intersection, delete the current polygon and return
            deleteCurrentPolygon();
            popUp->setPopupText("Полигоны не могут пересекаться");
            popUp->show();
            return;
        }
    }

    // create a QSpinBox and set it's position to the center of the circumscribed circle
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
    spinBox->setValue(polygonDifficulty);

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

    sceneAdjustCheck();

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

void MapScene::removePath() {
    foreach (QGraphicsLineItem* item, pathItems) {
        delete item;
    }
    pathItems = QList<QGraphicsLineItem*>();
}

void MapScene::addPathItem(QGraphicsLineItem* item) {
    pathItems << item;
    addItem(item);
}

void MapScene::getStartMarkPos(QPointF& startMarkPos) {
    if (startMarkItem == nullptr) {
        return;
    }
    startMarkPos.setX(startMarkItem->pos().x());
    startMarkPos.setY(startMarkItem->pos().y());
}

void MapScene::getFinishMarkPos(QPointF& finishMarkPos) {
    if (finishMarkItem == nullptr) {
        return;
    }
    finishMarkPos.setX(finishMarkItem->pos().x());
    finishMarkPos.setY(finishMarkItem->pos().y());
}

int MapScene::getDifficulty(qreal worldPositionX, qreal worldPositionY, qreal diameter){
    QPointF point1, point2, point3, point4;
    int maxValue = 0;

    point1.setX(worldPositionX);
    point1.setY(worldPositionY);
    point2.setX(worldPositionX + diameter);
    point2.setY(worldPositionY);
    point3.setX(worldPositionX);
    point3.setY(worldPositionY + diameter);
    point4.setX(worldPositionX + diameter);
    point4.setY(worldPositionY + diameter);

    QPolygonF rectangle;
    rectangle << point1 << point2 << point3 << point4;

    int polygonNum = polygonItems.length();
    for(int i = 0; i < polygonNum;i++){
        QGraphicsPolygonItem* curPolygon = polygonItems[i];
        QSpinBox* curSpinBox = qobject_cast<QSpinBox*>(spinBoxes[i]->widget());
        if(curPolygon->polygon().intersects(rectangle)){
            if (curSpinBox->value() > maxValue) {
                maxValue = curSpinBox->value();
            }
        }
    }
    return maxValue;
}

bool MapScene::gridStart(qreal worldPositionX, qreal worldPositionY,qreal diameter){
    QRectF gridNode;
    QPointF startPoint;
    getStartMarkPos(startPoint);
    startPoint.setX(startPoint.x() + 20);
    startPoint.setY(startPoint.y() + 25);
    gridNode.setLeft(worldPositionX);
    gridNode.setTop(worldPositionY);
    gridNode.setRight(worldPositionX+diameter);
    gridNode.setBottom(worldPositionY+diameter);
    return (gridNode.contains(startPoint));
}

bool MapScene::gridFinish(qreal worldPositionX, qreal worldPositionY,qreal diameter){
    QRectF gridNode;
    QPointF finishPoint;
    getFinishMarkPos(finishPoint);
    finishPoint.setX(finishPoint.x() + 20);
    finishPoint.setY(finishPoint.y() + 25);
    gridNode.setLeft(worldPositionX);
    gridNode.setTop(worldPositionY);
    gridNode.setRight(worldPositionX+diameter);
    gridNode.setBottom(worldPositionY+diameter);
    return (gridNode.contains(finishPoint));
}

QGraphicsPixmapItem* MapScene::getStartItem() {
    return startMarkItem;
}

QGraphicsPixmapItem* MapScene::getFinishItem() {
    return finishMarkItem;
}

double MapScene::getPathLength() {
    double pathLength = 0;
    QLineF currentLine;
    for (const QGraphicsLineItem* pathItem : pathItems) {
        currentLine = pathItem->line();
        pathLength += sqrt(pow(currentLine.dx(), 2) + pow(currentLine.dy(), 2));
    }
    return pathLength;
}

double MapScene::getPathTime() {
    double pathTime = 0;
    double curLineLength = 0;
    QPointF curLineCenter;
    double speed = 1;
    int obstacleValue = 0;
    QLineF curLine;
    for (const QGraphicsLineItem* pathItem : pathItems) {
        curLine = pathItem->line();
        curLineLength = sqrt(pow(curLine.dx(), 2) + pow(curLine.dy(), 2));
        curLineCenter = curLine.center();
        obstacleValue = getDifficulty(curLineCenter.x(), curLineCenter.y(), 1);
        pathTime += (curLineLength + curLineLength * (obstacleValue / 100.0)) / speed;
    }
    return pathTime;
}

void MapScene::saveMapSceneToXML(QFile& file) {

    if (!file.isOpen() || !file.isWritable()) {
        return;
    }

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);

    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("MapScene");

    xmlWriter.writeStartElement("Polygons");
    for (int i = 0; i < polygonItems.length(); i++) {
        xmlWriter.writeStartElement("Polygon");
        QGraphicsPolygonItem* polyItem = polygonItems[i];
        QPolygonF polygon = polyItem->polygon();

        int obstacleValue = qobject_cast<QSpinBox*>(spinBoxes[i]->widget())->value();
        xmlWriter.writeAttribute("ObstacleValue", QString::number(obstacleValue));

        for (const QPointF& point : polygon) {
            xmlWriter.writeStartElement("Point");
            xmlWriter.writeAttribute("x", QString::number(point.x()));
            xmlWriter.writeAttribute("y", QString::number(point.y()));
            xmlWriter.writeEndElement();
        }
        xmlWriter.writeEndElement();
    }
    xmlWriter.writeEndElement();

    xmlWriter.writeStartElement("Path");
    xmlWriter.writeAttribute("isDefined", pathItems.length() != 0? "true" : "false");
    xmlWriter.writeAttribute("Length", QString::number(getPathLength()));
    xmlWriter.writeAttribute("PassingTime", QString::number(getPathTime()));
    for (int i = 0; i < pathItems.length(); i++){
        QGraphicsLineItem* pathItem = pathItems[i];
        QLineF pathLine = pathItem->line();
        QPointF point1 = pathLine.p1();
        QPointF point2 = pathLine.p2();
        xmlWriter.writeStartElement("PathItem");
        xmlWriter.writeStartElement("Point");
        xmlWriter.writeAttribute("x", QString::number(point1.x()));
        xmlWriter.writeAttribute("y", QString::number(point1.y()));
        xmlWriter.writeEndElement();
        xmlWriter.writeStartElement("Point");
        xmlWriter.writeAttribute("x", QString::number(point2.x()));
        xmlWriter.writeAttribute("y", QString::number(point2.y()));
        xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();
    }
    xmlWriter.writeEndElement();

    QPointF startMarkPos;
    getStartMarkPos(startMarkPos);
    xmlWriter.writeStartElement("StartMark");
    xmlWriter.writeAttribute("isPlaced", getStartItem() != nullptr? "true" : "false");
    xmlWriter.writeStartElement("Position");
    xmlWriter.writeAttribute("x", QString::number(startMarkPos.x()));
    xmlWriter.writeAttribute("y", QString::number(startMarkPos.y()));
    xmlWriter.writeEndElement();
    xmlWriter.writeEndElement();

    QPointF finishMarkPos;
    getFinishMarkPos(finishMarkPos);
    xmlWriter.writeStartElement("FinishMark");
    xmlWriter.writeAttribute("isPlaced", getFinishItem() != nullptr? "true" : "false");
    xmlWriter.writeStartElement("Position");
    xmlWriter.writeAttribute("x", QString::number(finishMarkPos.x()));
    xmlWriter.writeAttribute("y", QString::number(finishMarkPos.y()));
    xmlWriter.writeEndElement();
    xmlWriter.writeEndElement();

    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
}

void MapScene::loadMapSceneFromXML(QFile& file) {
    clear();

    if (!file.isOpen() || !file.isReadable()) {
        return;
    }

    QXmlStreamReader xmlReader(&file);
    while (!xmlReader.atEnd()) {
        xmlReader.readNext();

        if (xmlReader.isStartElement()) {
            if (xmlReader.name().toString() == "StartMark") {
                bool isMarkPlaced = (xmlReader.attributes().value("isPlaced").toString() == "true");

                if (isMarkPlaced) {
                    float x = 0;
                    float y = 0;
                    while (!(xmlReader.isEndElement() && xmlReader.name().toString() == "StartMark")) {
                        xmlReader.readNext();
                        if (xmlReader.isStartElement() && xmlReader.name().toString() == "Position") {
                            x = xmlReader.attributes().value("x").toFloat();
                            y = xmlReader.attributes().value("y").toFloat();
                        }
                    }
                    QPixmap pixmap(":/svg/resources/startMarker.svg");
                    startMarkItem = new QGraphicsPixmapItem(pixmap);
                    startMarkItem->setScale(1.5);
                    addItem(startMarkItem);
                    QPointF markPos(x, y);
                    startMarkItem->setPos(markPos);
                }
            }
            else if (xmlReader.name().toString() == "FinishMark") {
                bool isMarkPlaced = (xmlReader.attributes().value("isPlaced").toString() == "true");

                if (isMarkPlaced) {
                    float x = 0;
                    float y = 0;
                    while (!(xmlReader.isEndElement() && xmlReader.name().toString() == "FinishMark")) {
                        xmlReader.readNext();
                        if (xmlReader.isStartElement() && xmlReader.name().toString() == "Position") {
                            x = xmlReader.attributes().value("x").toFloat();
                            y = xmlReader.attributes().value("y").toFloat();
                        }
                    }
                    QPixmap pixmap(":/svg/resources/finishMarker.svg");
                    finishMarkItem = new QGraphicsPixmapItem(pixmap);
                    finishMarkItem->setScale(1.5);
                    addItem(finishMarkItem);
                    QPointF markPos(x, y);
                    finishMarkItem->setPos(markPos);
                }
            }
            else if (xmlReader.name().toString() == "Polygons") {
                while (!(xmlReader.isEndElement() && xmlReader.name().toString() == "Polygons")) {
                    xmlReader.readNext();
                    if (xmlReader.isStartElement() && xmlReader.name().toString() == "Polygon") {
                        int obstacleValue = xmlReader.attributes().value("ObstacleValue").toInt();

                        while (!(xmlReader.isEndElement() && xmlReader.name().toString() == "Polygon")) {
                            xmlReader.readNext();
                            if (xmlReader.isStartElement() && xmlReader.name().toString() == "Point") {
                                float x = xmlReader.attributes().value("x").toFloat();
                                float y = xmlReader.attributes().value("y").toFloat();
                                QPointF polygonPoint(x, y);
                                currentPolygon << polygonPoint;
                            }
                        }
                        polygonItem = new QGraphicsPolygonItem(currentPolygon);
                        polygonItem->setBrush(Qt::gray);
                        addItem(polygonItem);
                        addCurrentPolygon(obstacleValue);
                    }
                }
            }
            else if (xmlReader.name().toString() == "Path") {
                bool isDefined = (xmlReader.attributes().value("isDefined").toString() == "true");
                if (!isDefined) {
                    continue;
                }

                while (!(xmlReader.tokenType() == QXmlStreamReader::EndElement && xmlReader.name().toString() == "Path")) {
                    xmlReader.readNext();

                    if (xmlReader.tokenType() == QXmlStreamReader::StartElement && xmlReader.name().toString() == "PathItem") {
                        QPointF point1, point2;

                        while (!(xmlReader.tokenType() == QXmlStreamReader::EndElement && xmlReader.name().toString() == "PathItem")) {
                            xmlReader.readNext();

                            if (xmlReader.tokenType() == QXmlStreamReader::StartElement && xmlReader.name().toString() == "Point") {
                                float x = xmlReader.attributes().value("x").toFloat();
                                float y = xmlReader.attributes().value("y").toFloat();

                                if (point1.isNull()) {
                                    point1.setX(x);
                                    point1.setY(y);
                                }
                                else {
                                    point2.setX(x);
                                    point2.setY(y);
                                }
                            }
                        }

                        QLineF curLine = QLineF(point1, point2);
                        QColor color = Qt::darkMagenta;
                        QPen pen(color, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
                        QGraphicsLineItem* curLineItem = new QGraphicsLineItem(curLine);
                        curLineItem->setPen(pen);
                        addPathItem(curLineItem);
                    }
                }
            }
        }
    }
}

void MapScene::clear() {
    QGraphicsScene::clear();
    polygonItems = QList<QGraphicsPolygonItem*>();
    spinBoxes = QList<QGraphicsProxyWidget*>();
    pathItems = QList<QGraphicsLineItem*>();
    currentPolygon = QPolygonF();
    startMarkItem = nullptr;
    finishMarkItem = nullptr;
    polygonItem = nullptr;
    currentLineItem = nullptr;
    sceneRectItem = nullptr;
}
