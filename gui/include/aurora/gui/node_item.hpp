#pragma once

#include <QGraphicsEllipseItem>
#include "aurora/core/node.hpp"

namespace aurora::gui {
class NodeItem : public QGraphicsEllipseItem {
public:
  explicit NodeItem(const aurora::Node& n, QGraphicsItem* parent = nullptr);
  aurora::NodeId id() const { return node_.id; }
private:
  aurora::Node node_;
};
} // namespace aurora::gui
