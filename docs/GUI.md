# AuroraGraph GUI

This GUI provides a minimal interface for viewing and querying graphs.

## Building

```
cmake -S . -B build
cmake --build build
```

## Usage

* **File → Open**: import `nodes.jsonl` and `edges.jsonl` files.
* **File → Save**: export the current graph to JSONL.
* **Query Tab**: write AGQL queries and press the Run toolbar button (or `Ctrl+R`).
* **Graph Tab**: shows a simple visualization of nodes and edges; use the Layout button to arrange nodes.
* **Theme**: toggle dark/light using the toolbar button.

Keyboard shortcuts: `Ctrl+O`, `Ctrl+S`, `Ctrl+R`, `Ctrl+L`.
