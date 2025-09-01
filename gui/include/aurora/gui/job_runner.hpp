#pragma once
#include <QObject>
#include <QThreadPool>
#include <memory>

#include "job.hpp"

namespace aurora::gui {
class JobRunner : public QObject {
public:
  explicit JobRunner(QObject* parent = nullptr);
  void post(std::unique_ptr<Job> job);
  void cancelAll();
private:
  QThreadPool pool_;
};
}
