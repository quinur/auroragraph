#pragma once

#include "aurora/algo/bfs.hpp"
#include "aurora/algo/pagerank.hpp"

namespace aurora::algo {

BfsResult parallel_bfs(const Graph& g, NodeId source, std::size_t num_threads);
PageRankResult parallel_pagerank(const Graph& g,
                                 double damping,
                                 std::size_t max_iter,
                                 double tol,
                                 std::size_t num_threads);

} // namespace aurora::algo

