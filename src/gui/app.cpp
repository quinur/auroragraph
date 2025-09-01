#include "aurora/gui/app.hpp"
#include <QApplication>
#include "aurora/gui/main_window.hpp"

namespace aurora::gui {
int App::run(int argc, char** argv) {
  QApplication app(argc, argv);
  MainWindow w;
  w.show();
  return app.exec();
}
}
