#include <gtest/gtest.h>

#include "aurora/agql/parser.hpp"
#include "aurora/agql/exec.hpp"

using namespace aurora;
using namespace aurora::agql;

TEST(AgqlExec, BasicQueries) {
  Graph g;
  // seed graph
  auto add_user = [&](int id, const std::string& name, int age){
    Properties p; p["id"] = Int(id); p["name"] = name; p["age"] = Int(age);
    g.add_node({"User"}, p);
  };
  add_user(1,"Ada",37);
  add_user(2,"Nicole",25);
  add_user(3,"Q",30);
  g.add_edge(1,2,{"FOLLOWS"}, {{"w", Real(1.0)}});
  g.add_edge(3,2,{"FOLLOWS"}, {{"w", Real(0.5)}});

  Executor ex(g);
  // Create edge via ids
  auto res = ex.run(parse_script("CREATE (a:User {id:1})-[:FOLLOWS]->(b:User {id:3});"));
  EXPECT_EQ(res.edges_created,1u);
  EXPECT_EQ(res.nodes_created,0u);

  auto r1 = ex.run(parse_script("MATCH (u:User {name:\"Q\"}) RETURN u, u.name;"));
  ASSERT_EQ(r1.rows.size(),1u);
  EXPECT_TRUE(std::holds_alternative<NodeId>(r1.rows[0].columns[0].second));
  EXPECT_EQ(std::get<std::string>(r1.rows[0].columns[1].second), "Q");

  auto r2 = ex.run(parse_script("MATCH (u:User)-[:FOLLOWS]->(v:User) WHERE v.name = \"Nicole\" RETURN u.name;"));
  std::vector<std::string> names;
  for(auto& row : r2.rows) names.push_back(std::get<std::string>(row.columns[0].second));
  std::sort(names.begin(), names.end());
  ASSERT_EQ(names.size(),2u);
  EXPECT_EQ(names[0],"Ada");
  EXPECT_EQ(names[1],"Q");

  auto r3 = ex.run(parse_script("MATCH (u:User) WHERE u.age >= 18 RETURN u.name;"));
  EXPECT_EQ(r3.rows.size(),3u);

  auto r4 = ex.run(parse_script("MATCH (u) WHERE u:User RETURN u;"));
  EXPECT_EQ(r4.rows.size(),3u);

  // type mismatch comparison -> false
  auto r5 = ex.run(parse_script("MATCH (u:User) WHERE u.name = 5 RETURN u;"));
  EXPECT_EQ(r5.rows.size(),0u);
}

