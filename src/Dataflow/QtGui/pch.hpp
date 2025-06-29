#pragma once

#include <Core/Utils/Color.hpp>
#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Node.hpp>
#include <Dataflow/Core/NodeFactory.hpp>
#include <Dataflow/Core/PortIn.hpp>
#include <Dataflow/Core/PortOut.hpp>
#include <Dataflow/RaDataflow.hpp>
#include <Engine/Data/EnvironmentTexture.hpp>
#include <Gui/ParameterSetEditor/ParameterSetEditor.hpp>
#include <Gui/Widgets/ControlPanel.hpp>
#include <PowerSlider/PowerSlider.hpp>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <QtNodes/ConnectionStyle>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtWidgets>
#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
