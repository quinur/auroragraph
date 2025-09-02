#include "aurora/agql/parser.hpp"
#include "aurora/agql/lexer.hpp"

#include <cctype>
#include <unordered_map>
#include <unordered_set>

namespace aurora::agql {

namespace {
struct Parser {
  std::vector<Token> toks;
  size_t i=0;

  explicit Parser(std::vector<Token> t) : toks(std::move(t)) {}

  const Token& peek() const { return toks[i]; }
  const Token& prev() const { return toks[i-1]; }
  bool check(TokenKind k) const { return peek().kind==k; }
  bool match(TokenKind k){ if(check(k)){ ++i; return true;} return false; }
  Token expect(TokenKind k, const char* msg){ if(!check(k)) error(msg); return toks[i++]; }
  [[noreturn]] void error(const std::string& msg) const { throw ParseError(msg, peek().line, peek().col); }

  Script parse_script() {
    Script s;
    while(!check(TokenKind::End)){
      s.stmts.push_back(parse_stmt());
      match(TokenKind::Semicolon);
    }
    return s;
  }

  Stmt parse_stmt(){
    if(match(TokenKind::Create)){
      NodePattern left = parse_node_pattern();
      if(match(TokenKind::Minus)){
        expect(TokenKind::LBracket, "[");
        RelPattern rel = parse_rel_body();
        expect(TokenKind::RBracket, "]");
        expect(TokenKind::Arrow, "->");
        NodePattern right = parse_node_pattern();
        return StmtCreateEdge{left, rel, right};
      }else{
        return StmtCreateNode{left};
      }
    } else if(match(TokenKind::Match)) {
      Pattern pat = parse_pattern();
      std::optional<Expr> where;
      if(match(TokenKind::Where)) where = parse_expr();
      std::vector<ReturnItem> items;
      if(match(TokenKind::Return)){
        items.push_back(parse_return_item());
        while(match(TokenKind::Comma)) items.push_back(parse_return_item());
      }
      // validate variable usage
      std::unordered_set<std::string> allowed;
      if(pat.left.var) allowed.insert(*pat.left.var);
      if(pat.right && pat.right->var) allowed.insert(*pat.right->var);
      if(pat.rel && pat.rel->var) allowed.insert(*pat.rel->var);
      auto check_vars=[&](const Expr& e){
        std::unordered_set<std::string> used;
        collect_vars(e, used);
        for(const auto& v:used) if(!allowed.count(v)) error("Unknown variable in expression");
      };
      if(where) check_vars(*where);
      for(const auto& it:items) check_vars(it.expr);
      return StmtMatch{pat, std::move(where), std::move(items)};
    } else if(match(TokenKind::Set)) {
      std::vector<SetItem> items;
      items.push_back(parse_set_item());
      while(match(TokenKind::Comma)) items.push_back(parse_set_item());
      return StmtSet{std::move(items)};
    } else if(match(TokenKind::Remove)) {
      std::vector<RemoveItem> items;
      items.push_back(parse_remove_item());
      while(match(TokenKind::Comma)) items.push_back(parse_remove_item());
      return StmtRemove{std::move(items)};
    } else if(match(TokenKind::Delete)) {
      bool detach = match(TokenKind::Detach);
      std::vector<std::string> vars;
      vars.push_back(expect(TokenKind::Ident, "variable").lexeme);
      while(match(TokenKind::Comma)) vars.push_back(expect(TokenKind::Ident, "variable").lexeme);
      return StmtDelete{detach, std::move(vars)};
    }
    error("Unknown statement");
  }

  Pattern parse_pattern(){
    NodePattern left = parse_node_pattern();
    if(match(TokenKind::Minus)){
      expect(TokenKind::LBracket, "[");
      RelPattern rel = parse_rel_body();
      expect(TokenKind::RBracket, "]");
      expect(TokenKind::Arrow, "->");
      NodePattern right = parse_node_pattern();
      return Pattern{left, rel, right};
    }
    return Pattern{left, std::nullopt, std::nullopt};
  }

  RelPattern parse_rel_body(){
    RelPattern r;
    if(match(TokenKind::Ident)){
      r.var = prev().lexeme;
    }
    if(match(TokenKind::Colon)) r.label = expect(TokenKind::Ident, "relationship type").lexeme;
    if(match(TokenKind::LBrace)){
      r.props = PropMap{};
      if(!check(TokenKind::RBrace)){
        r.props->items.push_back(parse_prop_pair());
        while(match(TokenKind::Comma)) r.props->items.push_back(parse_prop_pair());
      }
      expect(TokenKind::RBrace, "}");
    }
    return r;
  }

  NodePattern parse_node_pattern(){
    expect(TokenKind::LParen, "(");
    NodePattern n;
    if(check(TokenKind::Ident)){ n.var = peek().lexeme; i++; }
    if(match(TokenKind::Colon)) n.label = expect(TokenKind::Ident, "label").lexeme;
    if(match(TokenKind::LBrace)){
      n.props = PropMap{};
      if(!check(TokenKind::RBrace)){
        n.props->items.push_back(parse_prop_pair());
        while(match(TokenKind::Comma)) n.props->items.push_back(parse_prop_pair());
      }
      expect(TokenKind::RBrace, "}");
    }
    expect(TokenKind::RParen, ")");
    return n;
  }

  PropPair parse_prop_pair(){
    std::string key = expect(TokenKind::Ident, "prop key").lexeme;
    expect(TokenKind::Colon, ":");
    Scalar val = parse_literal();
    return PropPair{key, val};
  }

  Scalar parse_literal(){
    if(match(TokenKind::Null)) return Null{};
    if(match(TokenKind::True)) return true;
    if(match(TokenKind::False)) return false;
    if(match(TokenKind::Int)) return std::stoll(prev().lexeme);
    if(match(TokenKind::Real)) return std::stod(prev().lexeme);
    if(match(TokenKind::String)) return prev().lexeme;
    error("Expected literal");
  }

  ReturnItem parse_return_item(){
    Expr e = parse_expr();
    std::optional<std::string> alias;
    if(match(TokenKind::As)) alias = expect(TokenKind::Ident, "alias").lexeme;
    return ReturnItem{std::move(e), std::move(alias)};
  }

  SetItem parse_set_item(){
    std::string var = expect(TokenKind::Ident, "variable").lexeme;
    if(match(TokenKind::Dot)){
      std::string key = expect(TokenKind::Ident, "property").lexeme;
      expect(TokenKind::Equal, "=");
      Expr val = parse_expr();
      return SetProp{var, key, std::move(val)};
    } else if(match(TokenKind::Colon)){
      std::string label = expect(TokenKind::Ident, "label").lexeme;
      return SetAddLabel{var, label};
    }
    error("Invalid SET item");
  }

  RemoveItem parse_remove_item(){
    std::string var = expect(TokenKind::Ident, "variable").lexeme;
    if(match(TokenKind::Dot)){
      std::string key = expect(TokenKind::Ident, "property").lexeme;
      return RemoveProp{var, key};
    } else if(match(TokenKind::Colon)){
      std::string label = expect(TokenKind::Ident, "label").lexeme;
      return RemoveLabel{var, label};
    }
    error("Invalid REMOVE item");
  }

  // Expression parsing
  Expr parse_expr(){ return parse_or(); }
  Expr parse_or(){ Expr left = parse_and(); while(match(TokenKind::Or)){ Expr rhs = parse_and(); auto l=std::make_unique<Expr>(std::move(left)); auto r=std::make_unique<Expr>(std::move(rhs)); left = ExprOr{std::move(l), std::move(r)}; } return left; }
  Expr parse_and(){ Expr left = parse_not(); while(match(TokenKind::And)){ Expr rhs = parse_not(); auto l=std::make_unique<Expr>(std::move(left)); auto r=std::make_unique<Expr>(std::move(rhs)); left = ExprAnd{std::move(l), std::move(r)}; } return left; }
  Expr parse_not(){ if(match(TokenKind::Not)){ Expr e = parse_not(); auto ptr=std::make_unique<Expr>(std::move(e)); return ExprNot{std::move(ptr)}; } return parse_cmp(); }
  Expr parse_cmp(){ Expr left = parse_primary(); if(match(TokenKind::Equal)||match(TokenKind::NotEqual)||match(TokenKind::Less)||match(TokenKind::LessEq)||match(TokenKind::Greater)||match(TokenKind::GreaterEq)){ Token opTok = prev(); CmpOp op; switch(opTok.kind){ case TokenKind::Equal: op=CmpOp::Eq; break; case TokenKind::NotEqual: op=CmpOp::Ne; break; case TokenKind::Less: op=CmpOp::Lt; break; case TokenKind::LessEq: op=CmpOp::Le; break; case TokenKind::Greater: op=CmpOp::Gt; break; case TokenKind::GreaterEq: op=CmpOp::Ge; break; default: op=CmpOp::Eq; }
    Expr right = parse_primary(); auto l=std::make_unique<Expr>(std::move(left)); auto r=std::make_unique<Expr>(std::move(right)); return ExprCmp{std::move(l), op, std::move(r)}; } return left; }

  Expr parse_primary(){
    if(match(TokenKind::LParen)){ Expr e = parse_expr(); expect(TokenKind::RParen, ")"); return e; }
    if(match(TokenKind::Null)) return ExprLiteral{Null{}};
    if(match(TokenKind::True)) return ExprLiteral{true};
    if(match(TokenKind::False)) return ExprLiteral{false};
    if(match(TokenKind::Int)) return ExprLiteral{std::stoll(prev().lexeme)};
    if(match(TokenKind::Real)) return ExprLiteral{std::stod(prev().lexeme)};
    if(match(TokenKind::String)) return ExprLiteral{prev().lexeme};
    if(match(TokenKind::Param)) return ExprParam{prev().lexeme};
    if(match(TokenKind::Ident)){
      std::string first = prev().lexeme;
      if(match(TokenKind::Dot)){
        std::string key = expect(TokenKind::Ident, "property").lexeme;
        return ExprProp{first,key};
      } else if(match(TokenKind::Colon)) {
        std::string label = expect(TokenKind::Ident, "label").lexeme;
        return ExprLabelIs{first,label};
      } else {
        return ExprIdent{first};
      }
    }
    error("Unexpected token in expression");
  }

  void collect_vars(const Expr& e, std::unordered_set<std::string>& out){
    std::visit([&](auto&& node){
      using T=std::decay_t<decltype(node)>;
      if constexpr(std::is_same_v<T,ExprIdent>){ out.insert(node.name); }
      else if constexpr(std::is_same_v<T,ExprProp>){ out.insert(node.var); }
      else if constexpr(std::is_same_v<T,ExprLabelIs>){ out.insert(node.var); }
      else if constexpr(std::is_same_v<T,ExprParam>){ /* parameters introduce no vars */ }
      else if constexpr(std::is_same_v<T,ExprCmp>){ collect_vars(*node.lhs,out); collect_vars(*node.rhs,out); }
      else if constexpr(std::is_same_v<T,ExprNot>){ collect_vars(*node.e,out); }
      else if constexpr(std::is_same_v<T,ExprAnd>){ collect_vars(*node.lhs,out); collect_vars(*node.rhs,out); }
      else if constexpr(std::is_same_v<T,ExprOr>){ collect_vars(*node.lhs,out); collect_vars(*node.rhs,out); }
    }, e);
  }
};
} // namespace

Script parse_script(std::string_view input){
  Parser p{lex(input)};
  return p.parse_script();
}

} // namespace aurora::agql

