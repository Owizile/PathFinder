#include "mapeditor.h"
#include "./ui_mapeditor.h"
#include "pathfinding.h"
#include <QFileDialog>
#include <QDateTime>
#include <QDir>

MapEditor::MapEditor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MapEditor)
{
    ui->setupUi(this);
    mapScene = new MapScene();
    ui->mapView->setScene(mapScene);
    ui->statusBar->showMessage("Текущий режим: " + mapScene->getModeName());
}

MapEditor::~MapEditor() {
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
    ui->statusBar->showMessage("Текущий режим: " + mapScene->getModeName());
}

void MapEditor::on_startMarkButton_clicked() {
    mapScene->toggleMode(StartMarkMode);
    ui->statusBar->showMessage("Текущий режим: " + mapScene->getModeName());
}


void MapEditor::on_finishMarkButton_clicked() {
    mapScene->toggleMode(FinishMarkMode);
    ui->statusBar->showMessage("Текущий режим: " + mapScene->getModeName());
}

void MapEditor::on_startAlgorithmButton_clicked() {
    QGraphicsPixmapItem* start = mapScene->getStartItem();
    QGraphicsPixmapItem* finish = mapScene->getFinishItem();
    if(start && finish){
        QRectF sceneRect = mapScene->sceneRect();
        float gridNodeRadius = 3;
        grid.start(gridNodeRadius, sceneRect, mapScene);
        PathFinding pathFinder = PathFinding(grid);
        grid.path = pathFinder.findPath();
        grid.drawPath(mapScene);
    }
}

void MapEditor::on_loadFile_triggered() {
    QString mapFilePath = QFileDialog::getOpenFileName(this, tr("Load Map"), "", tr("XML Files (*.xml)"));
    if (mapFilePath == ""){
        return;
    }

    QFile mapFile(mapFilePath);
    mapFile.open(QIODevice::ReadOnly);
    mapScene->loadMapSceneFromXML(mapFile);
    mapFile.close();
}

void MapEditor::on_saveFile_triggered() {
    QString path("saves\\");
    QDir dir;

    if (!dir.exists(path)){
        dir.mkpath(path);
    }

    QFile outf(path + ("SavedMap " + (QDateTime::currentDateTime()).toString("dd MM yyyy hh-mm-ss")) + ".xml");
    outf.open(QIODevice::WriteOnly);
    mapScene->saveMapSceneToXML(outf);
    outf.close();
}

void MapEditor::on_saveFileAs_triggered() {
    QString path = QFileDialog::getSaveFileName(this, "Save As", "map.xml", "XML Files (*.xml)");
    if (path.isEmpty()) {
        return;
    }

    QFile outf(path);
    outf.open(QIODevice::WriteOnly);
    mapScene->saveMapSceneToXML(outf);
    outf.close();
}
