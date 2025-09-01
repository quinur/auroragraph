#include "aurora/gui/result_table.hpp"
#include <QStandardItemModel>
#include "aurora/agql/exec.hpp"

using namespace aurora;

namespace aurora::gui {
ResultTable::ResultTable(QWidget* parent) : QTableView(parent) {
  setModel(new QStandardItemModel(this));
}

void ResultTable::setResult(const aurora::agql::QueryResult& res) {
  auto* m = qobject_cast<QStandardItemModel*>(model());
  m->clear();
  if (res.rows.empty()) return;
  const auto& cols = res.rows.front().columns;
  for (int c = 0; c < static_cast<int>(cols.size()); ++c) {
    m->setHorizontalHeaderItem(c, new QStandardItem(QString::fromStdString(cols[c].first)));
  }
  int r = 0;
  for (const auto& row : res.rows) {
    m->setRowCount(r + 1);
    for (int c = 0; c < static_cast<int>(row.columns.size()); ++c) {
      const auto& val = row.columns[c].second;
      QString text;
      if (std::holds_alternative<std::monostate>(val)) text = "null";
      else if (auto p = std::get_if<int64_t>(&val)) text = QString::number(*p);
      else if (auto p = std::get_if<double>(&val)) text = QString::number(*p);
      else if (auto p = std::get_if<bool>(&val)) text = *p ? "true" : "false";
      else if (auto p = std::get_if<std::string>(&val)) text = QString::fromStdString(*p);
      else if (auto p = std::get_if<NodeId>(&val)) text = QString("#%1").arg(*p);
      m->setItem(r, c, new QStandardItem(text));
    }
    ++r;
  }
  resizeColumnsToContents();
}
} // namespace aurora::gui
