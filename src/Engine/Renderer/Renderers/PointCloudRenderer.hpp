#ifndef RADIUMENGINE_POINTCLOUDRENDERER_HPP
#define RADIUMENGINE_POINTCLOUDRENDERER_HPP

#include <Engine/Renderer/Renderer.hpp>

namespace Ra {
namespace Engine {

class PointCloudRenderer : public Ra::Engine::Renderer
{
public:
    struct VertexPickingQuery {
        VertexPickingQuery(int xCoord, int yCoord, bool b) :
            x(xCoord), y(yCoord), add(b) {}
        int  x;
        int  y;
        bool add;
    };
    struct VertexPickingResult {
        VertexPickingResult(int i, bool b) :
            idx(i), add(b) {}
        int  idx;
        bool add;
    };
public:
    PointCloudRenderer(uint width, uint height);
    virtual ~PointCloudRenderer();

////////////////////////////////////////////////////////////////////////////////
// Radium rendering
////////////////////////////////////////////////////////////////////////////////
public:
    virtual std::string getRendererName() const override;

    virtual void initializeInternal() override;
    virtual void resizeInternal() override;

    virtual void updateStepInternal(const Ra::Engine::RenderData& renderData) override;
    virtual void renderInternal(const Ra::Engine::RenderData& renderData) override;
    virtual void postProcessInternal(const Ra::Engine::RenderData& renderData) override;
    virtual void debugInternal(const Ra::Engine::RenderData& renderData) override;
    virtual void uiInternal(const Ra::Engine::RenderData& renderData) override;

    inline void setSingleLight(std::shared_ptr<Ra::Engine::Light> light);

private:
    std::shared_ptr<Ra::Engine::Light>   m_light;

    std::unique_ptr<Ra::Engine::FBO>     m_fbo;
    std::unique_ptr<Ra::Engine::Texture> m_texture;

    std::unique_ptr<Ra::Engine::FBO>     m_oitFbo;
    std::unique_ptr<Ra::Engine::Texture> m_accum;
    std::unique_ptr<Ra::Engine::Texture> m_revealage;

    std::vector<RenderObjectPtr> m_meshRO;
    std::vector<RenderObjectPtr> m_pointCloudRO;

////////////////////////////////////////////////////////////////////////////////
// Internal
////////////////////////////////////////////////////////////////////////////////
private:
    void initBuffers();
    void initShaders();

////////////////////////////////////////////////////////////////////////////////
// Rendering parameters
////////////////////////////////////////////////////////////////////////////////
public:
    inline void setPointSize(Scalar size);
    inline void setMainTransparency(Scalar t);
    inline void setLighting(Scalar lighting);
    inline void setLineWidth(Scalar lineWidth);
    inline void setLineTransparency(Scalar t);
    inline void setVectorLength(Scalar l);
    inline void setThreshold(Scalar t);
    inline void setBackgroundColor(Scalar r, Scalar g, Scalar b);
    inline void setCulling(bool b);
    inline void setFlatShading(bool b);

private:
    Scalar m_pointSize;
    Scalar m_lineWidth;
    Scalar m_lighting;
    Scalar m_mainTransparency;
    Scalar m_lineTransparency;
    Scalar m_vectorLength;
    Scalar m_threshold;

    Core::Vector4 m_backgroundColor;

    bool m_culling;
    bool m_flatShading;

////////////////////////////////////////////////////////////////////////////////
// Picking
////////////////////////////////////////////////////////////////////////////////
public:
    inline std::vector<VertexPickingResult>& getVertexPickingResult();
    inline void addVertexPickingQuery(const VertexPickingQuery& query);

    inline void setMousePosition(const Core::Vector2& pos);
    inline void setBrushRadius(Scalar brushRadius);
    inline void addBrushRadius(int incr);

private:
    void doVertexPicking();

private:
    Core::Vector2 m_mousePosition;
    Scalar        m_brushRadius;

    std::unique_ptr<Texture>         m_vertexPickingTex;
    std::vector<VertexPickingQuery>  m_vertexPickingQuery;
    std::vector<VertexPickingResult> m_vertexPickingResult;

}; // class PointCloudRenderer

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Renderers/PointCloudRenderer.inl>

#endif // RADIUMENGINE_POINTCLOUDRENDERER_HPP
