#include "aurora/gui/graph_view.hpp"
#include "aurora/gui/node_item.hpp"
#include "aurora/gui/edge_item.hpp"

#include <QWheelEvent>
#include <cmath>
#include <cstddef>

using namespace aurora;

namespace aurora::gui {

GraphView::GraphView(QWidget* parent) : QGraphicsView(parent) {
  setScene(&scene_);
  setRenderHint(QPainter::Antialiasing);
}

void GraphView::setGraph(aurora::Graph* g) {
  g_ = g;
  rebuildScene();
}

void GraphView::rebuildScene() {
  scene_.clear();
  nodes_.clear();
  edges_.clear();
  if (!g_) return;
  int idx = 0;
  for (const auto& [id, node] : g_->nodes()) {
    auto item = new NodeItem(node);
    item->setRect(-10, -10, 20, 20);
    item->setFlag(QGraphicsItem::ItemIsMovable);
    item->setFlag(QGraphicsItem::ItemIsSelectable);
    item->setPos(idx * 30, 0);
    scene_.addItem(item);
    nodes_[id] = item;
    ++idx;
  }
  for (const auto& [id, edge] : g_->edges()) {
    auto s = nodes_[edge.src];
    auto d = nodes_[edge.dst];
    if (!s || !d) continue;
    auto item = new EdgeItem(edge, s, d);
    item->updatePosition();
    scene_.addItem(item);
    edges_[id] = item;
  }
}

void GraphView::applyLayout() {
  std::size_t n = nodes_.size();
  if (n == 0) return;
  double radius = 100.0;
  int i = 0;
  for (auto& [id, item] : nodes_) {
    double angle = 2 * M_PI * i / n;
    item->setPos(radius * std::cos(angle), radius * std::sin(angle));
    ++i;
  }
  for (auto& [id, e] : edges_) e->updatePosition();
}

} // namespace aurora::gui
