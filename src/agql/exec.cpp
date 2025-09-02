#include "aurora/agql/exec.hpp"

#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#include "aurora/common/value.hpp"

namespace aurora::agql {

namespace {
using Binding = Executor::Binding;

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

std::optional<NodeId> get_node_id(const Binding& b, const std::string& var){
  auto it = b.nodes.find(var);
  if(it==b.nodes.end()) return std::nullopt;
  return it->second;
}

std::optional<EdgeId> get_edge_id(const Binding& b, const std::string& var){
  auto it = b.edges.find(var);
  if(it==b.edges.end()) return std::nullopt;
  return it->second;
}

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

Scalar eval_simple(Graph& g, const Expr& e, const Binding& b, const Executor::Params& params){
  return std::visit([&](auto&& node)->Scalar{
    using T = std::decay_t<decltype(node)>;
    if constexpr(std::is_same_v<T,ExprIdent>){
      auto nid = get_node_id(b, node.name); if(!nid) return Null{}; return static_cast<int64_t>(*nid);
    } else if constexpr(std::is_same_v<T,ExprProp>){
      auto nid = get_node_id(b, node.var); if(!nid) return Null{}; return get_prop_scalar(g, *nid, node.key);
    } else if constexpr(std::is_same_v<T,ExprLiteral>){
      return node.value;
    } else if constexpr(std::is_same_v<T,ExprParam>){
      auto it = params.find(node.name);
      if(it==params.end()) return Null{};
      return from_value(it->second);
    } else if constexpr(std::is_same_v<T,ExprLabelIs>){
      auto nid = get_node_id(b, node.var); if(!nid) return false; return node_has_label(g, *nid, node.label);
    } else if constexpr(std::is_same_v<T,ExprCmp>){
      Scalar l = eval_simple(g, *node.lhs, b, params); Scalar r = eval_simple(g, *node.rhs, b, params); return compare_scalar(l,node.op,r);
    } else if constexpr(std::is_same_v<T,ExprNot>){
      return !is_truthy(eval_simple(g,*node.e,b,params));
    } else if constexpr(std::is_same_v<T,ExprAnd>){
      return is_truthy(eval_simple(g,*node.lhs,b,params)) && is_truthy(eval_simple(g,*node.rhs,b,params));
    } else if constexpr(std::is_same_v<T,ExprOr>){
      return is_truthy(eval_simple(g,*node.lhs,b,params)) || is_truthy(eval_simple(g,*node.rhs,b,params));
    } else {
      return Null{};
    }
  }, e);
}

bool eval_bool(Graph& g, const Expr& e, const Binding& b, const Executor::Params& params){
  Scalar s = eval_simple(g, e, b, params);
  if(std::holds_alternative<bool>(s)) return std::get<bool>(s);
  return is_truthy(s);
}

RowValue::Scalar project_expr(Graph& g, const Expr& e, const Binding& b, const Executor::Params& params){
  return std::visit([&](auto&& node) -> RowValue::Scalar {
    using T = std::decay_t<decltype(node)>;
    if constexpr (std::is_same_v<T, ExprIdent>) {
      auto nid = get_node_id(b, node.name);
      if (!nid) return RowValue::Scalar{};
      return *nid;
    } else if constexpr (std::is_same_v<T, ExprProp>) {
      auto nid = get_node_id(b, node.var);
      if (!nid) return RowValue::Scalar{};
      Scalar s = get_prop_scalar(g, *nid, node.key);
      return std::visit([](auto&& v) -> RowValue::Scalar {
        using U = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<U, Null>) return std::monostate{};
        else return v;
      }, s);
    } else if constexpr (std::is_same_v<T, ExprLiteral>) {
      return std::visit([](auto&& v) -> RowValue::Scalar {
        using U = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<U, Null>) return std::monostate{};
        else return v;
      }, node.value);
    } else if constexpr (std::is_same_v<T, ExprParam>) {
      auto it = params.find(node.name);
      if (it == params.end()) return RowValue::Scalar{};
      return std::visit([](auto&& v) -> RowValue::Scalar {
        using U = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<U, std::monostate>) return std::monostate{};
        else return v;
      }, it->second);
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
  return run(script, Params{});
}

QueryResult Executor::run(const Script& script, const Params& params){
  QueryResult res;
  std::vector<Binding> current(1); // start with single empty binding
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
        for(auto& b : bindings){ if(!s.where || eval_bool(g_, *s.where, b, params)) filtered.push_back(b); }
        if(!s.ret.empty()){
          for(auto& b : filtered){
            RowValue row; for(const auto& item : s.ret){ RowValue::Scalar val = project_expr(g_, item.expr, b, params); std::string name = item.alias?*item.alias:default_column_name(item.expr); row.columns.emplace_back(std::move(name), std::move(val)); } res.rows.push_back(std::move(row)); }
        }
        current = std::move(filtered);
      } else if constexpr(std::is_same_v<T,StmtSet>){
        std::unordered_map<NodeId, std::unordered_map<std::string, Scalar>> updates;
        for(auto& b : current){
          for(const auto& item : s.items){
            std::visit(overloaded{
              [&](const SetProp& sp){
                auto nid = get_node_id(b, sp.var); if(!nid) return; Scalar val = eval_simple(g_, sp.value, b, params); updates[*nid][sp.key]=val; },
              [&](const SetAddLabel& sl){
                auto nid = get_node_id(b, sl.var); if(!nid) return; Node* n = g_.get_node(*nid); if(!n) return; if(std::find(n->labels.begin(), n->labels.end(), sl.label)==n->labels.end()) n->labels.push_back(sl.label); }
            }, item);
          }
        }
        for(auto& [nid, props] : updates){
          Node* n = g_.get_node(nid); if(!n) continue; for(auto& kv:props){ n->props[kv.first] = to_value(kv.second); }
        }
      } else if constexpr(std::is_same_v<T,StmtRemove>){
        std::unordered_map<NodeId, std::vector<std::string>> props;
        std::unordered_map<NodeId, std::vector<std::string>> labels;
        for(auto& b: current){
          for(const auto& item : s.items){
            std::visit(overloaded{
              [&](const RemoveProp& rp){ auto nid=get_node_id(b,rp.var); if(!nid) return; props[*nid].push_back(rp.key); },
              [&](const RemoveLabel& rl){ auto nid=get_node_id(b,rl.var); if(!nid) return; labels[*nid].push_back(rl.label); }
            }, item);
          }
        }
        for(auto& [nid, keys] : props){ if(Node* n=g_.get_node(nid)) for(const auto& k:keys) n->props.erase(k); }
        for(auto& [nid, labs] : labels){ if(Node* n=g_.get_node(nid)) for(const auto& l:labs){ auto it=std::find(n->labels.begin(), n->labels.end(), l); if(it!=n->labels.end()) n->labels.erase(it); } }
      } else if constexpr(std::is_same_v<T,StmtDelete>){
        std::unordered_set<NodeId> nodes;
        std::unordered_set<EdgeId> edges;
        for(auto& b: current){
          for(const auto& var : s.vars){
            if(auto nid = get_node_id(b,var)) nodes.insert(*nid);
            if(auto eid = get_edge_id(b,var)) edges.insert(*eid);
          }
        }
        for(EdgeId eid:edges) g_.remove_edge(eid);
        for(NodeId nid : nodes){
          if(!s.detach){
            if(!g_.out_edges(nid).empty() || !g_.in_edges(nid).empty()) throw std::runtime_error("Cannot delete node with edges");
          }
          g_.remove_node(nid);
        }
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

std::vector<std::pair<std::string, std::string>> Executor::list_indexes() const {
  std::vector<std::pair<std::string, std::string>> out;
  for (const auto& [k, idx] : indexes_) out.emplace_back(k.label, k.key);
  return out;
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
      Binding b; if(pat.left.var) b.nodes[*pat.left.var]=id; binds.push_back(std::move(b));
    }
  } else {
    for(NodeId src : left_nodes){
      for(EdgeId eid : g_.out_edges(src)){
        const Edge* e = g_.get_edge(eid); if(!e) continue;
        if(pat.rel->label && std::find(e->labels.begin(), e->labels.end(), *pat.rel->label)==e->labels.end()) continue;
        if(pat.rel->props && !match_edge_props(*e, *pat.rel->props)) continue;
        NodeId dst = e->dst;
        if(pat.right && !match_node(dst,*pat.right)) continue;
        Binding b; if(pat.left.var) b.nodes[*pat.left.var]=src; if(pat.rel->var) b.edges[*pat.rel->var]=eid; if(pat.right && pat.right->var) b.nodes[*pat.right->var]=dst; binds.push_back(std::move(b));
      }
    }
  }
  return binds;
}

} // namespace aurora::agql

