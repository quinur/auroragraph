#include "aurora/gui/edge_item.hpp"
#include "aurora/gui/node_item.hpp"

namespace aurora::gui {
EdgeItem::EdgeItem(const aurora::Edge& e, NodeItem* src, NodeItem* dst, QGraphicsItem* parent)
    : QGraphicsLineItem(parent), edge_(e), src_(src), dst_(dst) {}

void EdgeItem::updatePosition() {
  if (src_ && dst_) {
    setLine(QLineF(src_->pos(), dst_->pos()));
  }
}
}
