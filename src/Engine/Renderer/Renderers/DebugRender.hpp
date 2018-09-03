#ifndef RADIUMENGINE_DEBUGRENDERER_HPP
#define RADIUMENGINE_DEBUGRENDERER_HPP

#include <Engine/RaEngine.hpp>

#include <memory>
#include <vector>

#include <Core/Containers/VectorArray.hpp>
#include <Core/Utils/Singleton.hpp>

namespace Ra {
namespace Engine {
class Mesh;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {

/// The DebugRender class is a helper class to debug rendering.
class RA_ENGINE_API DebugRender {
    RA_SINGLETON_INTERFACE( DebugRender );

  public:
    DebugRender();

    virtual ~DebugRender();

    /// Initialize OpenGL stuff (shaders, uniform locations...)
    void initialize();

    /// Render the all the meshes for the given camera matrices.
    void render( const Core::Matrix4& view, const Core::Matrix4& proj );

    /// Add the segment { \p from, \p to } for render with color \p color.
    void addLine( const Core::Vector3& from, const Core::Vector3& to, const Core::Color& color );

    /// Add the point \p p for render with color \p color.
    void addPoint( const Core::Vector3& p, const Core::Color& color );

    /// Add the points in \p p for render with color \p color.
    void addPoints( const Core::Vector3Array& p, const Core::Color& color );

    /// Add the point \p p for render with colors from \p colors.
    /// \note There must be one color per point.
    void addPoints( const Core::Vector3Array& p, const Core::Vector4Array& colors );

    /// Add the mesh \p mesh for render with transformation \p transform.
    void addMesh( const std::shared_ptr<Mesh>& mesh,
                  const Core::Transform& transform = Core::Transform::Identity() );

    /// Add a 3D cross for render at \p position, of length \p size and color \p color.
    void addCross( const Core::Vector3& position, Scalar size, const Core::Color& color );

    /// Add a 3D sphere for render, centered at \p center, of radius \p radius and color \p color.
    void addSphere( const Core::Vector3& center, Scalar radius, const Core::Color& color );

    /// Add a 2D circle for render, centered at \p center in the plane tangent
    /// to \p normal, of radius \p radius and color \p color.
    void addCircle( const Core::Vector3& center, const Core::Vector3& normal, Scalar radius,
                    const Core::Color& color );

    /// Add 3 arrows for render, representing the frame given by \p transform, with length \p size.
    void addFrame( const Core::Transform& transform, Scalar size );

    /// Add the triangle { \p p0, \p p1, \p p2 } for render with color \p color.
    void addTriangle( const Core::Vector3& p0, const Core::Vector3& p1, const Core::Vector3& p2,
                      const Core::Color& color );

    /// Add a cube for render, representing the AABB \p box with color \p color.
    void addAABB( const Core::Aabb& box, const Core::Color& color );

    /// Add a cube for render, representing the AABB \p box oriented according
    /// to \p transform, with color \p color.
    void addOBB( const Core::Aabb& box, const Core::Transform& transform,
                 const Core::Color& color );

  private:
    /// Stores a segment data.
    struct Line {
        Line( const Core::Vector3& la, const Core::Vector3& lb, const Core::Color& lcol ) :
            a( la ),
            b( lb ),
            col( lcol ) {}

        Core::Vector3 a, b; ///< The segment endpoints.
        Core::Color col;    ///< The segment color.
    };

    /// Stores a point data.
    struct Point {
        Core::Vector3 p; ///< The point position.
        Core::Vector3 c; ///< The point color.
    };

    /// Stores a mesh data.
    struct DbgMesh {
        std::shared_ptr<Mesh> mesh; ///< The mesh.
        Core::Transform transform;  ///< The transform for the mesh.
    };

    /// Render all segments.
    void renderLines( const Core::Matrix4f& view, const Core::Matrix4f& proj );

    /// Render all points.
    void renderPoints( const Core::Matrix4f& view, const Core::Matrix4f& proj );

    /// Render all meshes.
    void renderMeshes( const Core::Matrix4f& view, const Core::Matrix4f& proj );

  private:
    uint m_lineProg;  ///< Index of the shader program used to draw lines.
    uint m_pointProg; ///< Index of the shader program used to draw points.
    uint m_meshProg;  ///< Index of the shader program used to draw meshes.

    uint m_modelLineLoc; ///< Uniform location for the model matrix used for line.
    uint m_viewLineLoc;  ///< Uniform location for the view matrix used for lines.
    uint m_projLineLoc;  ///< Uniform location for the projection matrix used for lines.

    uint m_viewPointLoc; ///< Uniform location for the view matrix used for points.
    uint m_projPointLoc; ///< Uniform location for the projection matrix used for points.

    uint m_modelMeshLoc; ///< Uniform location for the model matrix used for meshes.
    uint m_viewMeshLoc;  ///< Uniform location for the view matrix used for meshes.
    uint m_projMeshLoc;  ///< Uniform location for the projection matrix used for meshes.

    std::vector<Line> m_lines;     ///< The lines to render.
    std::vector<Point> m_points;   ///< The points to render.
    std::vector<DbgMesh> m_meshes; ///< The meshes to render.
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_DEBUGRENDERER_HPP
