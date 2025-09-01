# AuroraGraph CLI

The `aurora_cli` executable provides an interactive shell for running AGQL queries.

## Building

```bash
cmake -S . -B build
cmake --build build
```

## Running

```
aurora_cli [--data-nodes nodes.jsonl] [--data-edges edges.jsonl] [--snapshot file] [--wal file]
```

Use `:help` inside the shell to list available meta-commands. Examples:

```
:open nodes.jsonl edges.jsonl
:save nodes.jsonl edges.jsonl
:param countries ["US","DE"]
:params
:timing on
```

AGQL queries terminate with `;` at top level. Example:

```
MATCH (u:User)-[:FOLLOWS]->(v:User) WHERE v.name = "Nicole" RETURN u.name, v.name;
```

The CLI maintains history in `~/.aurora_history` and prints results in a simple table.
