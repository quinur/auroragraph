#include "aurora/gui/main_window.hpp"

#include <QAction>
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QSplitter>
#include <QStandardItemModel>
#include <QTabWidget>
#include <QToolBar>

#include "aurora/core/storage.hpp"
#include "aurora/gui/graph_view.hpp"
#include "aurora/gui/query_editor.hpp"
#include "aurora/gui/result_table.hpp"
#include "aurora/gui/status_bar.hpp"
#include "aurora/gui/dialogs.hpp"
#include "aurora/gui/job_runner.hpp"
#include "aurora/agql/parser.hpp"

namespace aurora::gui {

MainWindow::MainWindow() {
  setupUi();
  connectSignals();
  exec_ = std::make_unique<aurora::agql::Executor>(graph_);
  jobRunner_ = new JobRunner(this);
  applyTheme(true);
  refreshStats();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi() {
  setWindowTitle("AuroraGraphGUI");

  auto openAct = new QAction(QIcon(":/icons/open.svg"), tr("Open"), this);
  openAct->setShortcut(QKeySequence::Open);
  openAct->setObjectName("open");
  auto saveAct = new QAction(QIcon(":/icons/save.svg"), tr("Save"), this);
  saveAct->setShortcut(QKeySequence::Save);
  saveAct->setObjectName("save");
  auto runAct = new QAction(QIcon(":/icons/play.svg"), tr("Run"), this);
  runAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
  runAct->setObjectName("run");
  auto layoutAct = new QAction(QIcon(":/icons/layout.svg"), tr("Layout"), this);
  layoutAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
  layoutAct->setObjectName("layout");
  auto themeAct = new QAction(QIcon(":/icons/theme.svg"), tr("Theme"), this);
  themeAct->setObjectName("theme");

  QToolBar* tb = addToolBar(tr("Main"));
  tb->addAction(openAct);
  tb->addAction(saveAct);
  tb->addAction(runAct);
  tb->addAction(layoutAct);
  tb->addAction(themeAct);

  QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(openAct);
  fileMenu->addAction(saveAct);
  QMenu* queryMenu = menuBar()->addMenu(tr("&Query"));
  queryMenu->addAction(runAct);
  QMenu* viewMenu = menuBar()->addMenu(tr("&View"));
  viewMenu->addAction(layoutAct);
  viewMenu->addAction(themeAct);

  auto tabs = new QTabWidget(this);
  setCentralWidget(tabs);

  graphView_ = new GraphView;
  graphView_->setGraph(&graph_);
  tabs->addTab(graphView_, tr("Graph"));

  auto queryTab = new QWidget;
  auto splitter = new QSplitter(Qt::Horizontal, queryTab);
  queryEditor_ = new QueryEditor;
  resultTable_ = new ResultTable;
  splitter->addWidget(queryEditor_);
  splitter->addWidget(resultTable_);
  splitter->setStretchFactor(0, 3);
  splitter->setStretchFactor(1, 2);
  auto layout = new QHBoxLayout(queryTab);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(splitter);
  tabs->addTab(queryTab, tr("Query"));

  status_ = new StatusBar(this);
  setStatusBar(status_);
}

void MainWindow::connectSignals() {
  auto openAct = findChild<QAction*>("open");
  auto saveAct = findChild<QAction*>("save");
  auto runAct = findChild<QAction*>("run");
  auto layoutAct = findChild<QAction*>("layout");
  auto themeAct = findChild<QAction*>("theme");
  connect(openAct, &QAction::triggered, this, &MainWindow::openJsonl);
  connect(saveAct, &QAction::triggered, this, &MainWindow::saveJsonl);
  connect(runAct, &QAction::triggered, this, &MainWindow::runQuery);
  connect(layoutAct, &QAction::triggered, this, &MainWindow::applyLayout);
  connect(themeAct, &QAction::triggered, this, &MainWindow::toggleTheme);
}

void MainWindow::applyTheme(bool dark) {
  dark_ = dark;
  if (dark) {
    QFile f(":/qdark.qss");
    if (f.open(QIODevice::ReadOnly)) {
      qApp->setStyleSheet(QString::fromUtf8(f.readAll()));
    }
  } else {
    qApp->setStyleSheet({});
  }
}

void MainWindow::toggleTheme() { applyTheme(!dark_); }

void MainWindow::openJsonl() {
  QString nodes = dialogs::openFile(this, tr("Nodes (*.jsonl)"));
  if (nodes.isEmpty()) return;
  QString edges = dialogs::openFile(this, tr("Edges (*.jsonl)"));
  if (edges.isEmpty()) return;
  aurora::Storage::import_jsonl(graph_, nodes.toStdString(), edges.toStdString());
  graphView_->rebuildScene();
  refreshStats();
}

void MainWindow::saveJsonl() {
  QString nodes = dialogs::saveFile(this, tr("Nodes (*.jsonl)"));
  if (nodes.isEmpty()) return;
  QString edges = dialogs::saveFile(this, tr("Edges (*.jsonl)"));
  if (edges.isEmpty()) return;
  aurora::Storage::export_jsonl(graph_, nodes.toStdString(), edges.toStdString());
}

void MainWindow::runQuery() {
  QString text = queryEditor_->textCursor().selectedText();
  if (text.isEmpty()) text = queryEditor_->toPlainText();
  try {
    auto script = aurora::agql::parse_script(text.toStdString());
    auto res = exec_->run(script);
    resultTable_->setResult(res);
    graphView_->rebuildScene();
    refreshStats();
  } catch (const std::exception& e) {
    QMessageBox::critical(this, tr("AGQL error"), e.what());
  }
}

void MainWindow::applyLayout() {
  graphView_->applyLayout();
}

void MainWindow::refreshStats() {
  status_->setStats(graph_.node_count(), graph_.edge_count());
}

} // namespace aurora::gui
