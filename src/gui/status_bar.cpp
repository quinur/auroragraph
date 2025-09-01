#include "aurora/gui/status_bar.hpp"

namespace aurora::gui {
StatusBar::StatusBar(QWidget* parent) : QStatusBar(parent) {
  stats_ = new QLabel(this);
  addPermanentWidget(stats_);
}

void StatusBar::setStats(size_t nodes, size_t edges) {
  stats_->setText(QString("Nodes: %1 Edges: %2").arg(nodes).arg(edges));
}
}
