#include <gtest/gtest.h>

#include "aurora/agql/parser.hpp"
#include "aurora/agql/exec.hpp"

using namespace aurora;
using namespace aurora::agql;

TEST(AgqlMatchIndex, UsesIndexWhenAvailable) {
  Graph g;
  NodeId target = 0;
  for(int i=0;i<100;++i){
    Properties p; p["id"] = Int(i);
    NodeId n = g.add_node({"User"}, p);
    if(i==42) target=n;
  }

  Executor ex(g);
  auto script = parse_script("MATCH (u:User {id:42}) RETURN u;");
  auto r1 = ex.run(script);
  EXPECT_FALSE(ex.last_match_used_index());
  ASSERT_EQ(r1.rows.size(),1u);
  EXPECT_EQ(std::get<NodeId>(r1.rows[0].columns[0].second), target);

  ex.register_index("User","id");
  auto r2 = ex.run(script);
  EXPECT_TRUE(ex.last_match_used_index());
  ASSERT_EQ(r2.rows.size(),1u);
  EXPECT_EQ(std::get<NodeId>(r2.rows[0].columns[0].second), target);
}

