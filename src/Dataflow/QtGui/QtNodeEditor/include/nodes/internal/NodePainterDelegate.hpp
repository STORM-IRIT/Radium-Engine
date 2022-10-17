#pragma once

#include <QPainter>

#include "Export.hpp"
#include "NodeDataModel.hpp"
#include "NodeGeometry.hpp"

namespace QtNodes {

/// Class to allow for custom painting
class NODE_EDITOR_PUBLIC NodePainterDelegate
{

  public:
    virtual ~NodePainterDelegate() = default;

    virtual void
    paint( QPainter* painter, NodeGeometry const& geom, NodeDataModel const* model ) = 0;
};
} // namespace QtNodes
