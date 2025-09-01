#include "aurora/gui/agql_highlighter.hpp"
#include <QRegularExpression>

namespace aurora::gui {
AgqlHighlighter::AgqlHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent) {
  keywordFormat_.setForeground(Qt::cyan);
  keywordFormat_.setFontWeight(QFont::Bold);
  numberFormat_.setForeground(Qt::magenta);
  stringFormat_.setForeground(Qt::green);
  commentFormat_.setForeground(Qt::gray);
}

void AgqlHighlighter::highlightBlock(const QString& text) {
  static const QStringList keywords = {"MATCH", "RETURN", "WHERE", "CREATE", "DELETE", "SET"};
  for (const QString& kw : keywords) {
    QRegularExpression rx("\\b" + kw + "\\b", QRegularExpression::CaseInsensitiveOption);
    auto it = rx.globalMatch(text);
    while (it.hasNext()) {
      auto m = it.next();
      setFormat(m.capturedStart(), m.capturedLength(), keywordFormat_);
    }
  }
  QRegularExpression numRe("\\b[0-9]+(\\.[0-9]+)?\\b");
  auto it2 = numRe.globalMatch(text);
  while (it2.hasNext()) {
    auto m = it2.next();
    setFormat(m.capturedStart(), m.capturedLength(), numberFormat_);
  }
  QRegularExpression strRe("\"[^\"]*\"|'[^']*'");
  auto it3 = strRe.globalMatch(text);
  while (it3.hasNext()) {
    auto m = it3.next();
    setFormat(m.capturedStart(), m.capturedLength(), stringFormat_);
  }
  int idx = text.indexOf('#');
  if (idx >= 0) {
    setFormat(idx, text.length() - idx, commentFormat_);
  }
}
} // namespace aurora::gui
