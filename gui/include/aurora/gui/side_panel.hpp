#pragma once
#include <QDockWidget>

namespace aurora::gui {
class SidePanel : public QDockWidget {
public:
  explicit SidePanel(const QString& title, QWidget* parent = nullptr);
};
}
