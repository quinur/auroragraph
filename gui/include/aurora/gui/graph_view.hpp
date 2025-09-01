#pragma once

#include <QGraphicsView>
#include <unordered_map>

#include "aurora/core/graph.hpp"

namespace aurora::gui {
class NodeItem;
class EdgeItem;

class GraphView : public QGraphicsView {
public:
  explicit GraphView(QWidget* parent = nullptr);
  void setGraph(aurora::Graph* g);
  void rebuildScene();
  void applyLayout();
private:
  QGraphicsScene scene_;
  aurora::Graph* g_ = nullptr;
  std::unordered_map<aurora::NodeId, NodeItem*> nodes_;
  std::unordered_map<aurora::EdgeId, EdgeItem*> edges_;
};
} // namespace aurora::gui
