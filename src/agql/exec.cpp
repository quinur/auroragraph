#include "aurora/agql/exec.hpp"

#include <unordered_map>
#include <algorithm>

#include "aurora/common/value.hpp"

namespace aurora::agql {

namespace {
using Binding = std::unordered_map<std::string, NodeId>;

Value to_value(const Scalar& s) {
  return std::visit([](auto&& v)->Value{
    using T=std::decay_t<decltype(v)>; if constexpr(std::is_same_v<T,Null>) return std::monostate{}; else return v; }, s);
}

Scalar from_value(const Value& v) {
  return std::visit([](auto&& val)->Scalar{
    using T=std::decay_t<decltype(val)>; if constexpr(std::is_same_v<T,std::monostate>) return Null{}; else return Scalar(val); }, v);
}

Properties to_properties(const std::optional<PropMap>& pm) {
  Properties p;
  if(!pm) return p;
  for(const auto& pair: pm->items) p[pair.key] = to_value(pair.value);
  return p;
}

bool node_has_label(const Node& n, const std::string& label){
  return std::find(n.labels.begin(), n.labels.end(), label)!=n.labels.end();
}

bool value_eq(const Value& v, const Scalar& s){
  Scalar sv = from_value(v);
  if(sv.index()!=s.index()) return false;
  if(std::holds_alternative<Null>(s)) return true; // both null
  if(std::holds_alternative<int64_t>(s)) return std::get<int64_t>(sv)==std::get<int64_t>(s);
  if(std::holds_alternative<double>(s)) return std::get<double>(sv)==std::get<double>(s);
  if(std::holds_alternative<bool>(s)) return std::get<bool>(sv)==std::get<bool>(s);
  if(std::holds_alternative<std::string>(s)) return std::get<std::string>(sv)==std::get<std::string>(s);
  return false;
}

bool match_node_props(const Node& n, const PropMap& pm){
  for(const auto& pp : pm.items){
    auto it = n.props.find(pp.key);
    if(it==n.props.end()) return false;
    if(!value_eq(it->second, pp.value)) return false;
  }
  return true;
}

bool match_edge_props(const Edge& e, const PropMap& pm){
  for(const auto& pp : pm.items){
    auto it = e.props.find(pp.key);
    if(it==e.props.end()) return false;
    if(!value_eq(it->second, pp.value)) return false;
  }
  return true;
}

Scalar get_prop_scalar(Graph& g, NodeId id, const std::string& key){
  const Node* n = g.get_node(id);
  if(!n) return Null{};
  auto it = n->props.find(key);
  if(it==n->props.end()) return Null{};
  return from_value(it->second);
}

bool node_has_label(Graph& g, NodeId id, const std::string& label){
  const Node* n = g.get_node(id);
  return n && node_has_label(*n,label);
}

bool compare_scalar(const Scalar& a, CmpOp op, const Scalar& b){
  if(std::holds_alternative<Null>(a) || std::holds_alternative<Null>(b))
    return op==CmpOp::Eq && std::holds_alternative<Null>(a) && std::holds_alternative<Null>(b);
  if(a.index()!=b.index()) return false;
  if(std::holds_alternative<int64_t>(a)){
    auto av=std::get<int64_t>(a), bv=std::get<int64_t>(b);
    switch(op){case CmpOp::Eq:return av==bv;case CmpOp::Ne:return av!=bv;case CmpOp::Lt:return av<bv;case CmpOp::Le:return av<=bv;case CmpOp::Gt:return av>bv;case CmpOp::Ge:return av>=bv;}
  } else if(std::holds_alternative<double>(a)){
    auto av=std::get<double>(a), bv=std::get<double>(b);
    switch(op){case CmpOp::Eq:return av==bv;case CmpOp::Ne:return av!=bv;case CmpOp::Lt:return av<bv;case CmpOp::Le:return av<=bv;case CmpOp::Gt:return av>bv;case CmpOp::Ge:return av>=bv;}
  } else if(std::holds_alternative<bool>(a)){
    auto av=std::get<bool>(a), bv=std::get<bool>(b);
    switch(op){case CmpOp::Eq:return av==bv;case CmpOp::Ne:return av!=bv;default:return false;}
  } else if(std::holds_alternative<std::string>(a)){
    auto av=std::get<std::string>(a), bv=std::get<std::string>(b);
    switch(op){case CmpOp::Eq:return av==bv;case CmpOp::Ne:return av!=bv;case CmpOp::Lt:return av<bv;case CmpOp::Le:return av<=bv;case CmpOp::Gt:return av>bv;case CmpOp::Ge:return av>=bv;}
  }
  return false;
}

bool is_truthy(const Scalar& s){
  if(std::holds_alternative<Null>(s)) return false;
  if(std::holds_alternative<bool>(s)) return std::get<bool>(s);
  if(std::holds_alternative<int64_t>(s)) return std::get<int64_t>(s)!=0;
  if(std::holds_alternative<double>(s)) return std::get<double>(s)!=0.0;
  if(std::holds_alternative<std::string>(s)) return !std::get<std::string>(s).empty();
  return false;
}

Scalar eval_simple(Graph& g, const Expr& e, const Binding& b){
  return std::visit([&](auto&& node)->Scalar{
    using T = std::decay_t<decltype(node)>;
    if constexpr(std::is_same_v<T,ExprIdent>){
      auto it = b.find(node.name); if(it==b.end()) return Null{}; return static_cast<int64_t>(it->second); // but NodeId may exceed int64? NodeId is uint64, but variant uses NodeId later; for comparisons treat as int64 not needed. For simple, but for projection we handle separately.
    } else if constexpr(std::is_same_v<T,ExprProp>){
      auto it = b.find(node.var); if(it==b.end()) return Null{}; return get_prop_scalar(g, it->second, node.key);
    } else if constexpr(std::is_same_v<T,ExprLiteral>){
      return node.value;
    } else if constexpr(std::is_same_v<T,ExprLabelIs>){
      auto it = b.find(node.var); if(it==b.end()) return false; return node_has_label(g, it->second, node.label);
    } else if constexpr(std::is_same_v<T,ExprCmp>){
      Scalar l = eval_simple(g, *node.lhs, b); Scalar r = eval_simple(g, *node.rhs, b); return compare_scalar(l,node.op,r);
    } else if constexpr(std::is_same_v<T,ExprNot>){
      return !is_truthy(eval_simple(g,*node.e,b));
    } else if constexpr(std::is_same_v<T,ExprAnd>){
      return is_truthy(eval_simple(g,*node.lhs,b)) && is_truthy(eval_simple(g,*node.rhs,b));
    } else if constexpr(std::is_same_v<T,ExprOr>){
      return is_truthy(eval_simple(g,*node.lhs,b)) || is_truthy(eval_simple(g,*node.rhs,b));
    } else {
      return Null{};
    }
  }, e);
}

bool eval_bool(Graph& g, const Expr& e, const Binding& b){
  Scalar s = eval_simple(g, e, b);
  if(std::holds_alternative<bool>(s)) return std::get<bool>(s);
  return is_truthy(s);
}

RowValue::Scalar project_expr(Graph& g, const Expr& e, const Binding& b){
  return std::visit([&](auto&& node)->RowValue::Scalar{
    using T=std::decay_t<decltype(node)>;
    if constexpr(std::is_same_v<T,ExprIdent>){
      auto it = b.find(node.name); if(it==b.end()) return RowValue::Scalar{}; return it->second;
    } else if constexpr(std::is_same_v<T,ExprProp>){
      auto it = b.find(node.var); if(it==b.end()) return RowValue::Scalar{}; Scalar s = get_prop_scalar(g,it->second,node.key); return std::visit([](auto&& v)->RowValue::Scalar{ using U=std::decay_t<decltype(v)>; if constexpr(std::is_same_v<U,Null>) return std::monostate{}; else return v; }, s);
    } else if constexpr(std::is_same_v<T,ExprLiteral>){
      return std::visit([](auto&& v)->RowValue::Scalar{ using U=std::decay_t<decltype(v)>; if constexpr(std::is_same_v<U,Null>) return std::monostate{}; else return v; }, node.value);
    } else {
      return RowValue::Scalar{};
    }
  }, e);
}

std::string default_column_name(const Expr& e){
  return std::visit([](auto&& node)->std::string{
    using T=std::decay_t<decltype(node)>;
    if constexpr(std::is_same_v<T,ExprIdent>) return node.name;
    else if constexpr(std::is_same_v<T,ExprProp>) return node.var+"."+node.key;
    else return std::string{};
  }, e);
}

} // namespace

Executor::Executor(Graph& g) : g_(g) {}

QueryResult Executor::run(const Script& script){
  QueryResult res;
  for(const Stmt& st : script.stmts){
    std::visit([&](auto&& s){
      using T=std::decay_t<decltype(s)>;
      if constexpr(std::is_same_v<T,StmtCreateNode>){
        Properties props = to_properties(s.node.props);
        std::vector<std::string> labels; if(s.node.label) labels.push_back(*s.node.label);
        NodeId nid = g_.add_node(std::move(labels), std::move(props));
        const Node* n = g_.get_node(nid);
        for(auto& kv : indexes_) kv.second.on_node_added(*n);
        res.nodes_created++;
      } else if constexpr(std::is_same_v<T,StmtCreateEdge>){
        auto find_or_create = [&](const NodePattern& np){
          int64_t idval = 0; if(!np.props) throw std::runtime_error("id required");
          bool foundid=false; Properties p = to_properties(np.props);
          auto itid = p.find("id"); if(itid!=p.end()) { if(auto pi = as<Int>(itid->second)) { idval=*pi; foundid=true; } }
          if(!foundid) throw std::runtime_error("id required");
          for(NodeId nid=1; nid<=g_.node_count(); ++nid){
            const Node* n = g_.get_node(nid); if(!n) continue; auto it=n->props.find("id"); if(it!=n->props.end()) { if(auto pi=as<Int>(it->second); pi && *pi==idval) return nid; } }
          std::vector<std::string> labels; if(np.label) labels.push_back(*np.label);
          return g_.add_node(std::move(labels), std::move(p));
        };
        NodeId src = find_or_create(s.left);
        NodeId dst = find_or_create(s.right);
        std::vector<std::string> labels; if(s.rel.label) labels.push_back(*s.rel.label);
        Properties props = to_properties(s.rel.props);
        g_.add_edge(src,dst,std::move(labels),std::move(props));
        res.edges_created++;
      } else if constexpr(std::is_same_v<T,StmtMatch>){
        last_match_used_index_ = false;
        auto bindings = match_pattern(s.pattern);
        std::vector<Binding> filtered;
        for(auto& b : bindings){ if(!s.where || eval_bool(g_, *s.where, b)) filtered.push_back(b); }
        for(auto& b : filtered){
          RowValue row; for(const auto& item : s.ret){ RowValue::Scalar val = project_expr(g_, item.expr, b); std::string name = item.alias?*item.alias:default_column_name(item.expr); row.columns.emplace_back(std::move(name), std::move(val)); } res.rows.push_back(std::move(row)); }
      }
    }, st);
  }
  return res;
}

void Executor::register_index(const std::string& label, const std::string& key){
  IndexKey k{label, key};
  indexes_.erase(k);
  indexes_.emplace(k, Index(g_, label, key));
}

std::vector<NodeId> Executor::match_node_pattern(const NodePattern& np){
  if (np.label && np.props && np.props->items.size() == 1) {
    const auto& pp = np.props->items[0];
    IndexKey key{*np.label, pp.key};
    auto it = indexes_.find(key);
    if (it != indexes_.end()) {
      Value val = to_value(pp.value);
      auto ids = it->second.find(val);
      last_match_used_index_ = true;
      std::vector<NodeId> res;
      for (NodeId id : ids) {
        if (match_node(id, np)) res.push_back(id);
      }
      return res;
    }
  }
  std::vector<NodeId> res;
  for(NodeId id=1; id<=g_.node_count(); ++id){
    if(match_node(id,np)) res.push_back(id);
  }
  return res;
}

bool Executor::match_node(NodeId id, const NodePattern& np) const{
  const Node* n = g_.get_node(id); if(!n) return false;
  if(np.label && !node_has_label(*n,*np.label)) return false;
  if(np.props && !match_node_props(*n,*np.props)) return false;
  return true;
}

std::vector<Executor::Binding> Executor::match_pattern(const Pattern& pat){
  using Binding = Executor::Binding;
  std::vector<Binding> binds;
  auto left_nodes = match_node_pattern(pat.left);
  if(!pat.rel){
    for(NodeId id : left_nodes){
      Binding b; if(pat.left.var) b[*pat.left.var]=id; binds.push_back(std::move(b));
    }
  } else {
    for(NodeId src : left_nodes){
      for(EdgeId eid : g_.out_edges(src)){
        const Edge* e = g_.get_edge(eid); if(!e) continue;
        if(pat.rel->label && std::find(e->labels.begin(), e->labels.end(), *pat.rel->label)==e->labels.end()) continue;
        if(pat.rel->props && !match_edge_props(*e, *pat.rel->props)) continue;
        NodeId dst = e->dst;
        if(pat.right && !match_node(dst,*pat.right)) continue;
        Binding b; if(pat.left.var) b[*pat.left.var]=src; if(pat.right && pat.right->var) b[*pat.right->var]=dst; binds.push_back(std::move(b));
      }
    }
  }
  return binds;
}

} // namespace aurora::agql

