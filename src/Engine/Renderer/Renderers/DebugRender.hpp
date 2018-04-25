#ifndef RADIUMENGINE_DEBUGRENDERER_HPP
#define RADIUMENGINE_DEBUGRENDERER_HPP

#include <Engine/RaEngine.hpp>

#include <memory>
#include <vector>

#include <Core/Container/VectorArray.hpp>
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
    void render( const Core::Math::Matrix4& view, const Core::Math::Matrix4& proj );

    void addLine( const Core::Math::Vector3& from, const Core::Math::Vector3& to, const Core::Math::Color& color );
    void addPoint( const Core::Math::Vector3& p, const Core::Math::Color& color );
    void addPoints( const Core::Container::Vector3Array& p, const Core::Math::Color& color );
    void addPoints( const Core::Container::Vector3Array& p, const Core::Container::Vector4Array& colors );
    void addMesh( const std::shared_ptr<Mesh>& mesh,
                  const Core::Math::Transform& transform = Core::Math::Transform::Identity() );

    // Shortcuts
    void addCross( const Core::Math::Vector3& position, Scalar size, const Core::Math::Color& color );

    void addSphere( const Core::Math::Vector3& center, Scalar radius, const Core::Math::Color& color );

    void addCircle( const Core::Math::Vector3& center, const Core::Math::Vector3& normal, Scalar radius,
                    const Core::Math::Color& color );

    void addFrame( const Core::Math::Transform& transform, Scalar size );

    void addTriangle( const Core::Math::Vector3& p0, const Core::Math::Vector3& p1, const Core::Math::Vector3& p2,
                      const Core::Math::Color& color );

    void addAABB( const Core::Math::Aabb& box, const Core::Math::Color& color );

    void addOBB( const Core::Math::Aabb& box, const Core::Math::Transform& transform,
                 const Core::Math::Color& color );

  private:
    struct Line {
        Line( const Core::Math::Vector3& la, const Core::Math::Vector3& lb, const Core::Math::Color& lcol ) :
            a( la ),
            b( lb ),
            col( lcol ) {}

        Core::Math::Vector3 a, b;
        Core::Math::Color col;
    };

    struct Point {
        Core::Math::Vector3 p;
        Core::Math::Vector3 c;
    };

    struct DbgMesh {
        std::shared_ptr<Mesh> mesh;
        Core::Math::Transform transform;
    };

    void renderLines( const Core::Math::Matrix4f& view, const Core::Math::Matrix4f& proj );
    void renderPoints( const Core::Math::Matrix4f& view, const Core::Math::Matrix4f& proj );
    void renderMeshes( const Core::Math::Matrix4f& view, const Core::Math::Matrix4f& proj );

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
