# ----------------------------------------------------
# This file can be generated from a script:
# To do so, run "./generateFilelistForModule.sh Dataflow/Core"
# from ./scripts directory
# ----------------------------------------------------

set(dataflow_core_sources Core.cpp DataflowGraph.cpp Node.cpp NodeFactory.cpp Port.cpp)

set(dataflow_core_headers
    DataflowGraph.hpp
    EditableParameter.hpp
    GraphNodes.hpp
    Node.hpp
    NodeFactory.hpp
    Nodes/Functionals/BinaryOpNode.hpp
    Nodes/Functionals/CoreDataFunctionals.hpp
    Nodes/Functionals/FilterNode.hpp
    Nodes/Functionals/FunctionNode.hpp
    Nodes/Functionals/ReduceNode.hpp
    Nodes/Functionals/TransformNode.hpp
    Nodes/Sinks/CoreDataSinks.hpp
    Nodes/Sinks/SinkNode.hpp
    Nodes/Sources/CoreDataSources.hpp
    Nodes/Sources/FunctionSource.hpp
    Nodes/Sources/SingleDataSourceNode.hpp
    Port.hpp
    PortFactory.hpp
    PortIn.hpp
    PortOut.hpp
)
