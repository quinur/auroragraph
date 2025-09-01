#pragma once
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

namespace aurora::gui {
class AgqlHighlighter : public QSyntaxHighlighter {
public:
  explicit AgqlHighlighter(QTextDocument* parent = nullptr);
protected:
  void highlightBlock(const QString& text) override;
private:
  QTextCharFormat keywordFormat_;
  QTextCharFormat numberFormat_;
  QTextCharFormat stringFormat_;
  QTextCharFormat commentFormat_;
};
}
