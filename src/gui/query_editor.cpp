#include "aurora/gui/query_editor.hpp"
#include "aurora/gui/agql_highlighter.hpp"

namespace aurora::gui {
QueryEditor::QueryEditor(QWidget* parent) : QPlainTextEdit(parent) {
  highlighter_ = new AgqlHighlighter(document());
  setTabStopDistance(4 * fontMetrics().horizontalAdvance(' '));
}
}
