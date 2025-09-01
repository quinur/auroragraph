#include <gtest/gtest.h>
#include "aurora/agql/parser.hpp"
using namespace aurora::agql;

TEST(AgqlParserExt, ParseSetAndRemove) {
  auto script = parse_script("SET a.name = \"Ada\", a:Admin;");
  ASSERT_EQ(script.stmts.size(),1u);
  auto* st = std::get_if<StmtSet>(&script.stmts[0]);
  ASSERT_NE(st,nullptr);
  EXPECT_EQ(st->items.size(),2u);
}

TEST(AgqlParserExt, ParseRemove) {
  auto script = parse_script("REMOVE a.name, a:Admin;");
  auto* rm = std::get_if<StmtRemove>(&script.stmts[0]);
  ASSERT_NE(rm,nullptr);
  EXPECT_EQ(rm->items.size(),2u);
}

TEST(AgqlParserExt, ParseDeleteAndRelVar) {
  auto script = parse_script("MATCH (a)-[r:REL]->(b); DELETE DETACH a, r;");
  ASSERT_EQ(script.stmts.size(),2u);
  auto* m = std::get_if<StmtMatch>(&script.stmts[0]);
  ASSERT_NE(m,nullptr);
  ASSERT_TRUE(m->pattern.rel.has_value());
  ASSERT_TRUE(m->pattern.rel->var.has_value());
  EXPECT_EQ(*m->pattern.rel->var, "r");
  auto* del = std::get_if<StmtDelete>(&script.stmts[1]);
  ASSERT_NE(del,nullptr);
  EXPECT_TRUE(del->detach);
  EXPECT_EQ(del->vars.size(),2u);
}
