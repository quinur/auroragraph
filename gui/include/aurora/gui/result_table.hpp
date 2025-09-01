#pragma once
#include <QTableView>

namespace aurora { namespace agql { struct QueryResult; }}

namespace aurora::gui {
class ResultTable : public QTableView {
public:
  explicit ResultTable(QWidget* parent = nullptr);
  void setResult(const aurora::agql::QueryResult& res);
};
}
