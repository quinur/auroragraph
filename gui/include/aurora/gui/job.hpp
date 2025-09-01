#pragma once
#include <QRunnable>
#include <functional>
#include <QString>

namespace aurora::gui {
class Job : public QRunnable {
public:
  std::function<void()> fn;
  std::function<void()> on_done;
  std::function<void(const QString&)> on_error;
  void run() override;
};
}
