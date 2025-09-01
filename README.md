# AuroraGraph

AuroraGraph is a small, in-memory property graph engine implemented in C++20. It
supports labeled nodes and directed edges, each with arbitrary scalar properties.
The project aims to provide a compact foundation for experimenting with graph
algorithms and query processing.

## Building (Windows / Visual Studio 2022)

AuroraGraph now ships with a native Visual Studio solution. Open
`AuroraGraph.sln` to build the projects.

### Prerequisites

* Windows 10/11 x64
* Visual Studio 2022 with the **Desktop development with C++** workload
* Qt 6 for MSVC x64 and the *Qt Visual Studio Tools* extension

### Steps

1. Open the solution in Visual Studio.
2. Choose **Debug** or **Release** and build.
3. Executables are placed in `x64/<Config>/`. The GUI project runs `windeployqt`
   to copy required Qt DLLs.

The project is Windows-only and no longer uses CMake or bash scripts.

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
