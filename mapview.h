#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>

class MapView : public QGraphicsView {
public:
    MapView(QWidget* parent = nullptr) : QGraphicsView(parent) {}
protected:
    void wheelEvent(QWheelEvent* event) override;
};

#endif // MAPVIEW_H
