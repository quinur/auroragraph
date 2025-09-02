#include <cassert>
#include <iostream>
#include "aurora/core/graph.hpp"
#include "aurora/agql/parser.hpp"
#include "aurora/agql/exec.hpp"
#include "aurora/storage/snapshot.hpp"
#include "aurora/storage/recovery.hpp"

int main(){
    aurora::Graph g;
    // basic graph operations
    auto n1 = g.add_node({"User"}, {{"name", aurora::Text{"Ada"}}});
    auto n2 = g.add_node();

    g.add_edge(n1, n2, {"FOLLOWS"}, {{"w", aurora::Real{1.0}}});
    assert(g.node_count() == 2);
    assert(g.edge_count() == 1);

    // snapshot and recovery
    aurora::storage::Snapshot::write(g, "smoke.snap");
    aurora::Graph g2;
    aurora::storage::Snapshot::read(g2, "smoke.snap");
    assert(g2.node_count() == 2);
    assert(g2.edge_count() == 1);

    // simple AGQL execution
    aurora::agql::Executor exec(g2);
    auto script = aurora::agql::parse_script("MATCH (a)-[:FOLLOWS]->(b) RETURN a.id, b.id;");
    auto res = exec.run(script);
    assert(res.rows.size() == 1);

    std::cout << "smoke tests passed\n";
    return 0;
}
