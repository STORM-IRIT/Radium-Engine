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
class RA_ENGINE_API DebugRender {
    RA_SINGLETON_INTERFACE( DebugRender );

  public:
    DebugRender();
    virtual ~DebugRender();

    void initialize();
    void render( const Core::Matrix4& view, const Core::Matrix4& proj );

    void addLine( const Core::Vector3& from, const Core::Vector3& to, const Core::Color& color );
    void addPoint( const Core::Vector3& p, const Core::Color& color );
    void addPoints( const Core::Vector3Array& p, const Core::Color& color );
    void addPoints( const Core::Vector3Array& p, const Core::Vector4Array& colors );
    void addMesh( const std::shared_ptr<Mesh>& mesh,
                  const Core::Transform& transform = Core::Transform::Identity() );

    // Shortcuts
    void addCross( const Core::Vector3& position, Scalar size, const Core::Color& color );

    void addSphere( const Core::Vector3& center, Scalar radius, const Core::Color& color );

    void addCircle( const Core::Vector3& center, const Core::Vector3& normal, Scalar radius,
                    const Core::Color& color );

    void addFrame( const Core::Transform& transform, Scalar size );

    void addTriangle( const Core::Vector3& p0, const Core::Vector3& p1, const Core::Vector3& p2,
                      const Core::Color& color );

    void addAABB( const Core::Aabb& box, const Core::Color& color );

    void addOBB( const Core::Aabb& box, const Core::Transform& transform,
                 const Core::Color& color );

  private:
    struct Line {
        Line( const Core::Vector3& la, const Core::Vector3& lb, const Core::Color& lcol ) :
            a( la ),
            b( lb ),
            col( lcol ) {}

        Core::Vector3 a, b;
        Core::Color col;
    };

    struct Point {
        Core::Vector3 p;
        Core::Vector3 c;
    };

    struct DbgMesh {
        std::shared_ptr<Mesh> mesh;
        Core::Transform transform;
    };

    void renderLines( const Core::Matrix4f& view, const Core::Matrix4f& proj );
    void renderPoints( const Core::Matrix4f& view, const Core::Matrix4f& proj );
    void renderMeshes( const Core::Matrix4f& view, const Core::Matrix4f& proj );

  private:
    uint m_lineProg;
    uint m_pointProg;
    uint m_meshProg;

    uint m_modelLineLoc;
    uint m_viewLineLoc;
    uint m_projLineLoc;

    uint m_modelMeshLoc;
    uint m_viewMeshLoc;
    uint m_projMeshLoc;

    uint m_viewPointLoc;
    uint m_projPointLoc;

    std::vector<Line> m_lines;
    std::vector<DbgMesh> m_meshes;

    std::vector<Point> m_points;
};
} // namespace Engine
} // namespace Ra

#endif //RADIUMENGINE_DEBUGRENDERER_HPP
