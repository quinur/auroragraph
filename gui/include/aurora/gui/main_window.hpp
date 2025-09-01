#pragma once

#include <QMainWindow>
#include <memory>
#include <unordered_map>

#include "aurora/core/graph.hpp"
#include "aurora/agql/exec.hpp"

#include "status_bar.hpp"

namespace aurora::gui {
class GraphView;
class QueryEditor;
class ResultTable;
class JobRunner;

class MainWindow : public QMainWindow {
public:
  MainWindow();
  ~MainWindow() override;
  void openJsonl();
  void saveJsonl();
  void runQuery();
  void applyLayout();
  void toggleTheme();
private:
  void setupUi();
  void connectSignals();
  void applyTheme(bool dark);
  void refreshStats();

  aurora::Graph graph_;
  std::unique_ptr<aurora::agql::Executor> exec_;
  GraphView* graphView_{};
  QueryEditor* queryEditor_{};
  ResultTable* resultTable_{};
  JobRunner* jobRunner_{};
  StatusBar* status_{};
  bool dark_ = true;
};
}
