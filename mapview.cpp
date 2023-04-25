#include "mapview.h"

void MapView::wheelEvent(QWheelEvent* event) {
    event->ignore();
    event->accept();
}
