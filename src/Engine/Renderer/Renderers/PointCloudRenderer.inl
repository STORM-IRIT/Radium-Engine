namespace Ra {
namespace Engine {

////////////////////////////////////////////////////////////////////////////////
// Radium rendering
////////////////////////////////////////////////////////////////////////////////

void PointCloudRenderer::setSingleLight(std::shared_ptr<Ra::Engine::Light> light)
{
    m_light = light;
}

////////////////////////////////////////////////////////////////////////////////
// Rendering parameters
////////////////////////////////////////////////////////////////////////////////

void PointCloudRenderer::setPointSize(Scalar size)
{
    m_pointSize = size;
}

void PointCloudRenderer::setMainTransparency(Scalar t)
{
    m_mainTransparency = t;
}

void PointCloudRenderer::setLighting(Scalar lighting)
{
    m_lighting = lighting;
}

void PointCloudRenderer::setLineWidth(Scalar lineWidth)
{
    m_lineWidth = lineWidth;
}

void PointCloudRenderer::setLineTransparency(Scalar t)
{
    m_lineTransparency = t;
}

void PointCloudRenderer::setVectorLength(Scalar l)
{
    m_vectorLength = l;
}

void PointCloudRenderer::setThreshold(Scalar t)
{
    m_threshold = t;
}

void PointCloudRenderer::setBackgroundColor(Scalar r, Scalar g, Scalar b)
{
    m_backgroundColor = Core::Vector4(r, g, b, 1.0);
}

void PointCloudRenderer::setCulling(bool b)
{
    m_culling = b;
}

void PointCloudRenderer::setFlatShading(bool b)
{
    m_flatShading = b;
}

////////////////////////////////////////////////////////////////////////////////
// Picking
////////////////////////////////////////////////////////////////////////////////

std::vector<PointCloudRenderer::VertexPickingResult>& PointCloudRenderer::getVertexPickingResult()
{
    return m_vertexPickingResult;
}

void PointCloudRenderer::addVertexPickingQuery(const PointCloudRenderer::VertexPickingQuery& query)
{
    m_vertexPickingQuery.push_back(query);
}

void PointCloudRenderer::setMousePosition(const Core::Vector2& pos)
{
    m_mousePosition[0] = pos[0];
    m_mousePosition[1] = m_height - pos[1];
}

void PointCloudRenderer::setBrushRadius(Scalar brushRadius)
{
    m_brushRadius = brushRadius;
}

void PointCloudRenderer::addBrushRadius(int incr)
{
    m_brushRadius = std::max<int>(0, m_brushRadius+5*incr);
}

} // namespace Engine
} // namespace Ra
