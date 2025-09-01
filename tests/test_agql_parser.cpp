#include <gtest/gtest.h>

#include "aurora/agql/parser.hpp"

using namespace aurora::agql;

TEST(AgqlParser, ParseCreateNode) {
  auto script = parse_script("CREATE (u:User {name:\"Ada\", age:37});");
  ASSERT_EQ(script.stmts.size(),1);
  auto* cn = std::get_if<StmtCreateNode>(&script.stmts[0]);
  ASSERT_NE(cn,nullptr);
  EXPECT_EQ(*cn->node.label, "User");
  ASSERT_TRUE(cn->node.props);
  EXPECT_EQ(cn->node.props->items.size(),2u);
}

TEST(AgqlParser, ParseCreateEdge) {
  auto script = parse_script("CREATE (a:User {id:1, name:\"Ada\"})-[:FOLLOWS {w:1.0}]->(b:User {id:2, name:\"Q\"});");
  auto* ce = std::get_if<StmtCreateEdge>(&script.stmts[0]);
  ASSERT_NE(ce,nullptr);
  EXPECT_EQ(*ce->rel.label, "FOLLOWS");
  ASSERT_TRUE(ce->rel.props);
  EXPECT_EQ(ce->rel.props->items.size(),1u);
  EXPECT_TRUE(ce->left.props.has_value());
  EXPECT_TRUE(ce->right.props.has_value());
}

TEST(AgqlParser, ParseMatchNode) {
  auto script = parse_script("MATCH (u:User {name:\"Ada\"}) RETURN u, u.name;");
  auto* m = std::get_if<StmtMatch>(&script.stmts[0]);
  ASSERT_NE(m,nullptr);
  ASSERT_FALSE(m->pattern.rel.has_value());
  EXPECT_EQ(m->ret.size(),2u);
}

TEST(AgqlParser, ParseMatchWhereAlias) {
  auto script = parse_script(
    "MATCH (a:User {name:\"Ada\"})-[:FOLLOWS]->(b:User)\n"
    "WHERE b.name = \"Q\" AND NOT (a:User AND b:User)\n"
    "RETURN a AS src, b.name AS dstName;");
  auto* m = std::get_if<StmtMatch>(&script.stmts[0]);
  ASSERT_NE(m,nullptr);
  ASSERT_TRUE(m->pattern.rel.has_value());
  ASSERT_TRUE(m->where.has_value());
  EXPECT_EQ(m->ret.size(),2u);
  EXPECT_TRUE(m->ret[0].alias.has_value());
}

TEST(AgqlParser, NegativeTests) {
  EXPECT_THROW(parse_script("CREATE (u:User {name:\"Ada\"};"), ParseError);
  EXPECT_THROW(parse_script("MATCH (u) RETURN"), ParseError);
  EXPECT_THROW(parse_script("CREATE (u); foo"), ParseError);
  EXPECT_THROW(parse_script("MATCH (a) WHERE b.name = 1 RETURN a;"), ParseError);
}

