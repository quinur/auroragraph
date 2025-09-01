#pragma once
#include <QString>
class QWidget;

namespace aurora::gui::dialogs {
QString openFile(QWidget* parent, const QString& filter);
QString saveFile(QWidget* parent, const QString& filter);
}
