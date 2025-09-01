#pragma once
#include <QStatusBar>
#include <QLabel>

namespace aurora::gui {
class StatusBar : public QStatusBar {
public:
  explicit StatusBar(QWidget* parent = nullptr);
  void setStats(size_t nodes, size_t edges);
private:
  QLabel* stats_;
};
}
