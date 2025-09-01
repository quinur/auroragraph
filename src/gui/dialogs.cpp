#include "aurora/gui/dialogs.hpp"
#include <QFileDialog>
#include <QObject>

namespace aurora::gui::dialogs {
QString openFile(QWidget* parent, const QString& filter) {
  return QFileDialog::getOpenFileName(parent, QObject::tr("Open"), QString(), filter);
}
QString saveFile(QWidget* parent, const QString& filter) {
  return QFileDialog::getSaveFileName(parent, QObject::tr("Save"), QString(), filter);
}
}
