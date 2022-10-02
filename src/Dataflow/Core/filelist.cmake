# ----------------------------------------------------
# This file can be generated from a script:
# To do so, run "./generateFilelistForModule.sh Dataflow/Core"
# from ./scripts directory
# ----------------------------------------------------

set(dataflow_core_sources DataflowGraph.cpp Node.cpp NodeFactory.cpp Nodes/CoreBuiltInsNodes.cpp)

set(dataflow_core_headers
    DataflowGraph.hpp
    EditableParameter.hpp
    Enumerator.hpp
    Node.hpp
    NodeFactory.hpp
    Nodes/CoreBuiltInsNodes.hpp
    Nodes/Filters/CoreDataFilters.hpp
    Nodes/Filters/FilterNode.hpp
    Nodes/Sinks/CoreDataSinks.hpp
    Nodes/Sinks/SinkNode.hpp
    Nodes/Sources/CoreDataSources.hpp
    Nodes/Sources/SingleDataSourceNode.hpp
    Port.hpp
    TypeDemangler.hpp
)

set(dataflow_core_inlines
    DataflowGraph.inl
    EditableParameter.inl
    Enumerator.inl
    Node.inl
    NodeFactory.inl
    Nodes/Filters/FilterNode.inl
    Nodes/Sinks/CoreDataSinks.inl
    Nodes/Sinks/SinkNode.inl
    Nodes/Sources/CoreDataSources.inl
    Nodes/Sources/SingleDataSourceNode.inl
    Port.inl
    TypeDemangler.inl
)
