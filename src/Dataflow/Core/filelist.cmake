# ----------------------------------------------------
# This file can be generated from a script:
# To do so, run "./generateFilelistForModule.sh Dataflow/Core"
# from ./scripts directory
# ----------------------------------------------------

set(dataflow_core_sources Core.cpp DataflowGraph.cpp Node.cpp NodeFactory.cpp Port.cpp)

set(dataflow_core_headers
    DataflowGraph.hpp
    EditableParameter.hpp
    Functionals/BinaryOpNode.hpp
    Functionals/CoreDataFunctionals.hpp
    Functionals/FilterNode.hpp
    Functionals/FunctionNode.hpp
    Functionals/ReduceNode.hpp
    Functionals/TransformNode.hpp
    GraphNodes.hpp
    Node.hpp
    NodeFactory.hpp
    Port.hpp
    PortFactory.hpp
    PortIn.hpp
    PortOut.hpp
    Sinks/CoreDataSinks.hpp
    Sinks/SinkNode.hpp
    Sources/CoreDataSources.hpp
    Sources/FunctionSource.hpp
    Sources/SingleDataSourceNode.hpp
)
