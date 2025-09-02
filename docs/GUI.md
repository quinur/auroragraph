# AuroraGraph GUI

This GUI provides a minimal interface for viewing and querying graphs.

## Building

Use Visual Studio 2022 to build the **AuroraGraphGUI** project from
`AuroraGraph.sln`. If you have the Qt Visual Studio Tools extension installed,
select a Qt 6 MSVC kit in the project settings. Otherwise, set the `QTDIR`
environment variable to the root of your Qt installation. The executable is
placed in `x64/<Config>/AuroraGraphGUI.exe`, and a post‑build step runs
`windeployqt` to copy the required Qt DLLs.

## Usage

* **File → Open**: import `nodes.jsonl` and `edges.jsonl` files.
* **File → Save**: export the current graph to JSONL.
* **Query Tab**: write AGQL queries and press the Run toolbar button (or `Ctrl+R`).
* **Graph Tab**: shows a simple visualization of nodes and edges; use the Layout button to arrange nodes.
* **Theme**: toggle dark/light using the toolbar button.

Keyboard shortcuts: `Ctrl+O`, `Ctrl+S`, `Ctrl+R`, `Ctrl+L`.
