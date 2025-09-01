# AuroraGraph

AuroraGraph is a small, in-memory property graph engine implemented in C++20. It
supports labeled nodes and directed edges, each with arbitrary scalar properties.
The project aims to provide a compact foundation for experimenting with graph
algorithms and query processing.

## Building

```bash
./install.sh
```

This script configures and builds the project in `build/` and runs the unit tests.
If Qt6 is not available, the GUI target is skipped.

## JSONL Format

**nodes.jsonl**
```json
{"id":1,"labels":["User"],"props":{"name":"Ada","age":37}}
{"id":2,"labels":["User"],"props":{"name":"Q"}}
```

**edges.jsonl**
```json
{"id":1,"src":1,"dst":2,"labels":["FOLLOWS"],"props":{"w":0.5}}
```

## Roadmap

- AGQL query language and REPL
- Graph algorithms (BFS/DFS/Dijkstra/PageRank)
- Indexes and simple optimizer
- Write-ahead logging and snapshots
