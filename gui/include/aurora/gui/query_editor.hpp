#pragma once
#include <QPlainTextEdit>

namespace aurora::gui {
class AgqlHighlighter;
class QueryEditor : public QPlainTextEdit {
public:
  explicit QueryEditor(QWidget* parent = nullptr);
private:
  AgqlHighlighter* highlighter_;
};
}
