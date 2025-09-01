#pragma once

#include <string>

namespace aurora::agql {

enum class TokenKind {
  End,
  // keywords
  Create, Match, Where, Return, And, Or, Not, As,
  Set, Delete, Detach, Remove,
  // symbols
  LParen, RParen, LBrace, RBrace, LBracket, RBracket,
  Colon, Comma, Dot, Semicolon,
  Arrow, Minus,
  Equal, NotEqual, Less, LessEq, Greater, GreaterEq,
  // literals
  String, Int, Real, True, False, Null,
  Ident
};

struct Token {
  TokenKind kind;
  std::string lexeme;
  size_t line;
  size_t col;
};

} // namespace aurora::agql

