#include "aurora/gui/node_item.hpp"

namespace aurora::gui {
NodeItem::NodeItem(const aurora::Node& n, QGraphicsItem* parent)
    : QGraphicsEllipseItem(parent), node_(n) {}
}
