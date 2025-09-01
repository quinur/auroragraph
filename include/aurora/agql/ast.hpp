#pragma once

#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace aurora::agql {

struct Null {};
using Scalar = std::variant<Null, int64_t, double, bool, std::string>;

struct Expr; // forward
using ExprPtr = std::unique_ptr<Expr>;

// Expressions
struct ExprIdent { std::string name; };
struct ExprProp  { std::string var; std::string key; };
struct ExprLabelIs { std::string var; std::string label; };
struct ExprLiteral { Scalar value; };

enum class CmpOp { Eq, Ne, Lt, Le, Gt, Ge };
struct ExprCmp { ExprPtr lhs; CmpOp op; ExprPtr rhs; };
struct ExprNot { ExprPtr e; };
struct ExprAnd { ExprPtr lhs, rhs; };
struct ExprOr  { ExprPtr lhs, rhs; };

struct Expr : std::variant<ExprIdent, ExprProp, ExprLabelIs, ExprLiteral,
                           ExprCmp, ExprNot, ExprAnd, ExprOr> {
  using std::variant<ExprIdent, ExprProp, ExprLabelIs, ExprLiteral,
                     ExprCmp, ExprNot, ExprAnd, ExprOr>::variant;
};

// Node/edge patterns
struct PropPair { std::string key; Scalar value; };
struct PropMap { std::vector<PropPair> items; };

struct NodePattern {
  std::optional<std::string> var;
  std::optional<std::string> label;
  std::optional<PropMap> props;
};

struct RelPattern {
  std::optional<std::string> label;
  std::optional<PropMap> props;
};

struct Pattern {
  NodePattern left;
  std::optional<RelPattern> rel;
  std::optional<NodePattern> right;
};

// Statements
struct StmtCreateNode { NodePattern node; };
struct StmtCreateEdge { NodePattern left; RelPattern rel; NodePattern right; };

struct ReturnItem {
  Expr expr;
  std::optional<std::string> alias;
};
struct StmtMatch {
  Pattern pattern;
  std::optional<Expr> where;
  std::vector<ReturnItem> ret;
};

using Stmt = std::variant<StmtCreateNode, StmtCreateEdge, StmtMatch>;

struct Script { std::vector<Stmt> stmts; };

} // namespace aurora::agql

