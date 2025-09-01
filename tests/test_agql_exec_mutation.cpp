#include <gtest/gtest.h>
#include "aurora/agql/parser.hpp"
#include "aurora/agql/exec.hpp"
#include <algorithm>

using namespace aurora;
using namespace aurora::agql;

TEST(AgqlExecMutation, BasicMutations) {
  Graph g;
  auto add_user = [&](int id, const std::string& name){
    Properties p; p["id"] = Int(id); p["name"] = name; g.add_node({"User"}, p);
  };
  add_user(1,"Alice");
  add_user(2,"Bob");
  g.add_edge(1,2,{"REL"},{});

  Executor ex(g);

  // property set
  ex.run(parse_script("MATCH (u:User {id:1}); SET u.name = \"Ada\";"));
  const Node* n1 = g.get_node(1);
  ASSERT_NE(n1,nullptr);
  EXPECT_EQ(std::get<std::string>(n1->props.at("name")), "Ada");

  // label add/remove
  ex.run(parse_script("MATCH (u:User {id:1}); SET u:VIP;"));
  n1 = g.get_node(1);
  EXPECT_NE(std::find(n1->labels.begin(), n1->labels.end(), "VIP"), n1->labels.end());
  ex.run(parse_script("MATCH (u:User {id:1}); REMOVE u:VIP;"));
  n1 = g.get_node(1);
  EXPECT_EQ(std::find(n1->labels.begin(), n1->labels.end(), "VIP"), n1->labels.end());

  // delete without detach fails
  EXPECT_THROW(ex.run(parse_script("MATCH (u:User {id:2}); DELETE u;")), std::runtime_error);

  // delete relationship
  ex.run(parse_script("MATCH (a:User {id:1})-[r:REL]->(b:User {id:2}); DELETE r;"));
  EXPECT_EQ(g.edge_count(),0u);

  // delete node with detach (edge re-added first)
  g.add_edge(1,2,{"REL"},{});
  ex.run(parse_script("MATCH (u:User {id:2}); DELETE DETACH u;"));
  EXPECT_FALSE(g.has_node(2));
  EXPECT_EQ(g.edge_count(),0u);
}
