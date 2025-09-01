#include <gtest/gtest.h>
#include "aurora/agql/lexer.hpp"
using namespace aurora::agql;

TEST(AgqlLexerExt, NewKeywords) {
  auto toks = lex("SET DELETE DETACH REMOVE");
  EXPECT_EQ(toks[0].kind, TokenKind::Set);
  EXPECT_EQ(toks[1].kind, TokenKind::Delete);
  EXPECT_EQ(toks[2].kind, TokenKind::Detach);
  EXPECT_EQ(toks[3].kind, TokenKind::Remove);
}
