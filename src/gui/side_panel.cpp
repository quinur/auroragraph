#include "aurora/gui/side_panel.hpp"

namespace aurora::gui {
SidePanel::SidePanel(const QString& title, QWidget* parent)
    : QDockWidget(title, parent) {}
}
