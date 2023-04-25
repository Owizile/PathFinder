#include "mapeditor.h"
#include "./ui_mapeditor.h"

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
