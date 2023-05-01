#include "mapeditor.h"
#include "./ui_mapeditor.h"
#include "grid.h"
#include "pathfinding.h"


MapEditor::MapEditor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MapEditor)
{
    ui->setupUi(this);
    mapScene = new MapScene();
    ui->mapView->setScene(mapScene);
}

MapEditor::~MapEditor()
{
    delete ui;
}

void MapEditor::on_addButton_clicked() {
    mapScene->addCurrentPolygon();
}

void MapEditor::on_deleteButton_clicked() {
    mapScene->deleteCurrentPolygon();
}

void MapEditor::on_editButton_clicked() {
    mapScene->toggleMode(EditMode);
}

void MapEditor::on_startMarkButton_clicked() {
    mapScene->toggleMode(StartMarkMode);
}


void MapEditor::on_finishMarkButton_clicked() {
    mapScene->toggleMode(FinishMarkMode);
}

void MapEditor::on_Button_Start_Algorithm_clicked()
{
    QGraphicsPixmapItem* start = mapScene->startItem();
    QGraphicsPixmapItem* finish = mapScene->finishItem();
    if((start) && (finish)){
        QRectF sceneRect = mapScene->sceneRect();
        mapScene->addRect(sceneRect);
        Grid grid;
        grid.start(2,sceneRect,mapScene);
        PathFinding pathFinder = PathFinding(grid);
        grid.path = pathFinder.findPath();
        grid.drawPath(mapScene);
    }
}

