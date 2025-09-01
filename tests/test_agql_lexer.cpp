#include <gtest/gtest.h>

#include "aurora/agql/lexer.hpp"

using namespace aurora::agql;

TEST(AgqlLexer, TokensAndEscapes) {
  auto toks = lex("CREATE MATCH \"str\" 123 1.5 -> != <= >=");
  EXPECT_EQ(toks[0].kind, TokenKind::Create);
  EXPECT_EQ(toks[1].kind, TokenKind::Match);
  EXPECT_EQ(toks[2].kind, TokenKind::String);
  EXPECT_EQ(toks[3].kind, TokenKind::Int);
  EXPECT_EQ(toks[4].kind, TokenKind::Real);
  EXPECT_EQ(toks[5].kind, TokenKind::Arrow);
  EXPECT_EQ(toks[6].kind, TokenKind::NotEqual);
  EXPECT_EQ(toks[7].kind, TokenKind::LessEq);
  EXPECT_EQ(toks[8].kind, TokenKind::GreaterEq);
}

TEST(AgqlLexer, CommentSkip) {
  auto toks = lex("MATCH -- hi\nRETURN");
  EXPECT_EQ(toks[0].kind, TokenKind::Match);
  EXPECT_EQ(toks[1].kind, TokenKind::Return);
}

TEST(AgqlLexer, UnterminatedString) {
  EXPECT_THROW(lex("\"oops"), LexError);
}

TEST(AgqlLexer, BadEscape) {
  EXPECT_THROW(lex("\"\\x\""), LexError);
}

