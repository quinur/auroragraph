#pragma once

#include <QGraphicsLineItem>
#include "aurora/core/edge.hpp"

namespace aurora::gui {
class NodeItem;
class EdgeItem : public QGraphicsLineItem {
public:
  EdgeItem(const aurora::Edge& e, NodeItem* src, NodeItem* dst, QGraphicsItem* parent = nullptr);
  aurora::EdgeId id() const { return edge_.id; }
  void updatePosition();
private:
  aurora::Edge edge_;
  NodeItem* src_;
  NodeItem* dst_;
};
} // namespace aurora::gui
