### Radium adaptation

Code source taken from https://github.com/paceholder/nodeeditor
Some adaptations were made to allow seamless integration with Radium dataflow graph.
  -> only used for graph editing (no evaluation with data propagation)
  -> externalization of load/save and NodeDataModel management operation.
  -> modified CMakeLists.txt and directory structure to keep only what is used in Radium

### Purpose

**NodeEditor** is conceived as a general-purpose Qt-based library aimed at
graph-controlled data processing.  Nodes represent algorithms with certain inputs
and outputs. Connections transfer data from the output (source) of the first node
to the input (sink) of the second one.

**NodeEditor** framework is a Visual [Dataflow
Programming](https://en.wikipedia.org/wiki/Dataflow_programming) tool.  A library
client defines models and registers them in the data model registry.  Further
work is driven by events taking place in DataModels and Nodes.  The model
computing is triggered upon arriving of any new input data. The computed result
is propagated to the output connections. Each new connection fetches available
data and propagates is further.

Each change in the source node is immediately propagated through all the
connections updating  the whole graph.

### Citing

    Dmitry Pinaev et al, Qt5 Node Editor, (2017), GitHub repository, https://github.com/paceholder/nodeeditor

BibTeX

    @misc{Pinaev2017,
      author = {Dmitry Pinaev et al},
      title = {Qt5 Node Editor},
      year = {2017},
      publisher = {GitHub},
      journal = {GitHub repository},
      howpublished = {\url{https://github.com/paceholder/nodeeditor}},
      commit = {1d1757d09b03cea0e4921bc19659465fe6e65b9b}
    }
