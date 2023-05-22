#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include <QMainWindow>
#include <QDebug>
#include "mapscene.h"
#include "grid.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MapEditor; }
QT_END_NAMESPACE

class MapEditor : public QMainWindow {
    Q_OBJECT

public:
    MapEditor(QWidget *parent = nullptr);
    ~MapEditor();

private slots:
    void on_deleteButton_clicked();
    void on_editButton_clicked();
    void on_addButton_clicked();
    void on_startMarkButton_clicked();
    void on_finishMarkButton_clicked();
    void on_startAlgorithmButton_clicked();
    void on_loadFile_triggered();
    void on_saveFile_triggered();
    void on_saveFileAs_triggered();

private:
    Ui::MapEditor *ui;
    MapScene *mapScene;
    Grid grid;
};
#endif // MAPEDITOR_H
