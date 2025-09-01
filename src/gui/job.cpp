#include "aurora/gui/job.hpp"

namespace aurora::gui {
void Job::run() {
  try {
    if (fn) fn();
    if (on_done) on_done();
  } catch (const std::exception& e) {
    if (on_error) on_error(QString::fromUtf8(e.what()));
  }
}
}
