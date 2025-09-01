#include "aurora/gui/job_runner.hpp"

namespace aurora::gui {
JobRunner::JobRunner(QObject* parent) : QObject(parent) {
  pool_.setMaxThreadCount(QThread::idealThreadCount());
}

void JobRunner::post(std::unique_ptr<Job> job) {
  pool_.start(job.release());
}

void JobRunner::cancelAll() {
  pool_.clear();
}
}
