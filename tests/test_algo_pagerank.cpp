#include <gtest/gtest.h>

#include "aurora/core/graph.hpp"
#include "aurora/algo/pagerank.hpp"

using namespace aurora;

TEST(PageRank, LineGraph) {
  Graph g;
  NodeId n1=g.add_node();
  NodeId n2=g.add_node();
  NodeId n3=g.add_node();
  g.add_edge(n1,n2);
  g.add_edge(n2,n3);

  auto res = pagerank(g);
  ASSERT_EQ(res.scores.size(),3u);
  double s1 = res.scores[n1];
  double s2 = res.scores[n2];
  double s3 = res.scores[n3];
  EXPECT_NEAR(s1,0.184,1e-2);
  EXPECT_NEAR(s2,0.341,1e-2);
  EXPECT_NEAR(s3,0.474,1e-2);
  EXPECT_NEAR(s1+s2+s3,1.0,1e-6);
}

TEST(PageRank, StarGraph) {
  Graph g;
  NodeId center = g.add_node();
  std::vector<NodeId> leaves;
  for(int i=0;i<4;++i){
    NodeId l=g.add_node();
    leaves.push_back(l);
    g.add_edge(l,center);
  }

  auto res = pagerank(g);
  EXPECT_GT(res.scores[center],0.5);
  for(NodeId l:leaves) EXPECT_LT(res.scores[l],0.2);
  double sum=0; for(auto [id,sc]:res.scores) sum+=sc; EXPECT_NEAR(sum,1.0,1e-6);
}

