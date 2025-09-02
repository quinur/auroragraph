#include "aurora/agql/lexer.hpp"

#include <cctype>
#include <charconv>

namespace aurora::agql {

namespace {
struct Lexer {
  std::string_view input;
  size_t pos = 0;
  size_t line = 1;
  size_t col = 1;
  std::vector<Token> toks;

  explicit Lexer(std::string_view in) : input(in) {}

  char peek() const {
    return pos < input.size() ? input[pos] : '\0';
  }
  char get() {
    char c = peek();
    if (c == '\n') {
      line++;
      col = 1;
    } else if (c != '\0') {
      col++;
    }
    pos++;
    return c;
  }

  void add(TokenKind k, std::string lex, size_t l, size_t c) {
    toks.push_back(Token{k, std::move(lex), l, c});
  }

  [[noreturn]] void error(const std::string &msg) {
    throw LexError(msg, line, col);
  }

  void skip_ws() {
    while (true) {
      char c = peek();
      if (c == '\0')
        return;
      if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
        get();
        continue;
      }
      if (c == '-' && pos + 1 < input.size() && input[pos + 1] == '-') {
        while (peek() && peek() != '\n')
          get();
        continue;
      }
      return;
    }
  }

  void lex_number() {
    size_t start_pos = pos;
    size_t l = line, c = col;
    bool is_real = false;
    if (peek() == '-')
      get();
    while (std::isdigit(peek()))
      get();
    if (peek() == '.') {
      is_real = true;
      get();
      if (!std::isdigit(peek()))
        error("Invalid real literal");
      while (std::isdigit(peek()))
        get();
      if (peek() == 'e' || peek() == 'E') {
        get();
        if (peek() == '+' || peek() == '-')
          get();
        if (!std::isdigit(peek()))
          error("Invalid real literal");
        while (std::isdigit(peek()))
          get();
      }
    }
    std::string lex(input.substr(start_pos, pos - start_pos));
    if (is_real) {
      double val;
      auto [p, ec] = std::from_chars(lex.data(), lex.data() + lex.size(), val);
      if (ec != std::errc{})
        error("Invalid real literal");
      add(TokenKind::Real, lex, l, c);
    } else {
      long long val;
      auto [p, ec] = std::from_chars(lex.data(), lex.data() + lex.size(), val);
      if (ec != std::errc{})
        error("Invalid int literal");
      add(TokenKind::Int, lex, l, c);
    }
  }

  void lex_string() {
    size_t l = line, c = col;
    get();
    std::string out;
    while (true) {
      char ch = peek();
      if (ch == '\0' || ch == '\n')
        error("Unterminated string");
      if (ch == '"') {
        get();
        break;
      }
      if (ch == '\\') {
        get();
        char esc = peek();
        if (esc == '"' || esc == '\\') {
          out.push_back(esc);
          get();
        } else
          error("Invalid escape");
      } else {
        out.push_back(ch);
        get();
      }
    }
    add(TokenKind::String, out, l, c);
  }

  void lex_ident() {
    size_t start = pos;
    size_t l = line, c = col;
    while (std::isalnum(peek()) || peek() == '_')
      get();
    std::string lexeme(input.substr(start, pos - start));
    std::string upper;
    upper.reserve(lexeme.size());
      for (char ch : lexeme)
        upper.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(ch))));
    if (upper == "CREATE")
      add(TokenKind::Create, lexeme, l, c);
    else if (upper == "MATCH")
      add(TokenKind::Match, lexeme, l, c);
    else if (upper == "WHERE")
      add(TokenKind::Where, lexeme, l, c);
    else if (upper == "RETURN")
      add(TokenKind::Return, lexeme, l, c);
    else if (upper == "AND")
      add(TokenKind::And, lexeme, l, c);
    else if (upper == "OR")
      add(TokenKind::Or, lexeme, l, c);
    else if (upper == "NOT")
      add(TokenKind::Not, lexeme, l, c);
    else if (upper == "AS")
      add(TokenKind::As, lexeme, l, c);
    else if (upper == "TRUE")
      add(TokenKind::True, lexeme, l, c);
    else if (upper == "FALSE")
      add(TokenKind::False, lexeme, l, c);
    else if (upper == "NULL")
      add(TokenKind::Null, lexeme, l, c);
    else if (upper == "SET")
      add(TokenKind::Set, lexeme, l, c);
    else if (upper == "DELETE")
      add(TokenKind::Delete, lexeme, l, c);
    else if (upper == "DETACH")
      add(TokenKind::Detach, lexeme, l, c);
    else if (upper == "REMOVE")
      add(TokenKind::Remove, lexeme, l, c);
    else if (upper == "DISTINCT")
      add(TokenKind::Distinct, lexeme, l, c);
    else if (upper == "ORDER")
      add(TokenKind::Order, lexeme, l, c);
    else if (upper == "BY")
      add(TokenKind::By, lexeme, l, c);
    else if (upper == "LIMIT")
      add(TokenKind::Limit, lexeme, l, c);
    else if (upper == "SKIP")
      add(TokenKind::Skip, lexeme, l, c);
    else if (upper == "OPTIONAL")
      add(TokenKind::Optional, lexeme, l, c);
    else if (upper == "WITH")
      add(TokenKind::With, lexeme, l, c);
    else if (upper == "UNWIND")
      add(TokenKind::Unwind, lexeme, l, c);
    else if (upper == "EXISTS")
      add(TokenKind::Exists, lexeme, l, c);
    else if (upper == "IS")
      add(TokenKind::Is, lexeme, l, c);
    else if (upper == "IN")
      add(TokenKind::In, lexeme, l, c);
    else if (upper == "STARTS")
      add(TokenKind::Starts, lexeme, l, c);
    else if (upper == "ENDS")
      add(TokenKind::Ends, lexeme, l, c);
    else if (upper == "CONTAINS")
      add(TokenKind::Contains, lexeme, l, c);
    else if (upper == "COUNT")
      add(TokenKind::Count, lexeme, l, c);
    else if (upper == "SUM")
      add(TokenKind::Sum, lexeme, l, c);
    else if (upper == "AVG")
      add(TokenKind::Avg, lexeme, l, c);
    else if (upper == "MIN")
      add(TokenKind::Min, lexeme, l, c);
    else if (upper == "MAX")
      add(TokenKind::Max, lexeme, l, c);
    else
      add(TokenKind::Ident, lexeme, l, c);
  }

  void run() {
    while (true) {
      skip_ws();
      size_t l = line, c = col;
      char ch = peek();
      if (ch == '\0')
        break;
      if (std::isalpha(ch) || ch == '_') {
        lex_ident();
        continue;
      }
      if (std::isdigit(ch) ||
          (ch == '-' && pos + 1 < input.size() && std::isdigit(input[pos + 1]))) {
        lex_number();
        continue;
      }
      switch (ch) {
      case '(':
        get();
        add(TokenKind::LParen, "(", l, c);
        break;
      case ')':
        get();
        add(TokenKind::RParen, ")", l, c);
        break;
      case '{':
        get();
        add(TokenKind::LBrace, "{", l, c);
        break;
      case '}':
        get();
        add(TokenKind::RBrace, "}", l, c);
        break;
      case '[':
        get();
        add(TokenKind::LBracket, "[", l, c);
        break;
      case ']':
        get();
        add(TokenKind::RBracket, "]", l, c);
        break;
      case ':':
        get();
        add(TokenKind::Colon, ":", l, c);
        break;
      case ',':
        get();
        add(TokenKind::Comma, ",", l, c);
        break;
      case '.':
        get();
        add(TokenKind::Dot, ".", l, c);
        break;
      case ';':
        get();
        add(TokenKind::Semicolon, ";", l, c);
        break;
      case '"':
        lex_string();
        break;
      case '+': {
        get();
        if (peek() == '=') {
          get();
          add(TokenKind::PlusEqual, "+=", l, c);
        } else {
          add(TokenKind::Plus, "+", l, c);
        }
        break;
      }
      case '-': {
        get();
        if (peek() == '>') {
          get();
          add(TokenKind::Arrow, "->", l, c);
        } else
          add(TokenKind::Minus, "-", l, c);
        break;
      }
      case '*':
        get();
        add(TokenKind::Star, "*", l, c);
        break;
      case '/':
        get();
        add(TokenKind::Slash, "/", l, c);
        break;
      case '=':
        get();
        add(TokenKind::Equal, "=", l, c);
        break;
      case '!': {
        get();
        if (peek() == '=') {
          get();
          add(TokenKind::NotEqual, "!=", l, c);
        } else
          error("Unexpected '!'");
        break;
      }
      case '<': {
        get();
        if (peek() == '=') {
          get();
          add(TokenKind::LessEq, "<=", l, c);
        } else {
          add(TokenKind::Less, "<", l, c);
        }
        break;
      }
      case '>': {
        get();
        if (peek() == '=') {
          get();
          add(TokenKind::GreaterEq, ">=", l, c);
        } else {
          add(TokenKind::Greater, ">", l, c);
        }
        break;
      }
      case '$': {
        get();
        if (!std::isalpha(peek()) && peek() != '_')
          error("Invalid parameter name");
        size_t start = pos;
        while (std::isalnum(peek()) || peek() == '_')
          get();
        std::string name(input.substr(start, pos - start));
        add(TokenKind::Param, name, l, c);
        break;
      }
      default:
        error(std::string("Unexpected character: ") + ch);
      }
    }
    add(TokenKind::End, "", line, col);
  }
};
} // namespace

std::vector<Token> lex(std::string_view input) {
  Lexer L(input);
  L.run();
  return L.toks;
}

} // namespace aurora::agql
