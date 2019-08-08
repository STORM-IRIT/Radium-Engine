#ifndef RADIUMENGINE_DEBUGRENDERER_HPP
#define RADIUMENGINE_DEBUGRENDERER_HPP

#include <Engine/RaEngine.hpp>

#include <memory>
#include <vector>

#include <Core/Containers/VectorArray.hpp>
#include <Core/Utils/Color.hpp>
#include <Core/Utils/Singleton.hpp>

namespace Ra::Engine {
class Mesh;

/** This allow to draw debug objects.
 * @todo : port this to a more Radium-style code
 */
class RA_ENGINE_API DebugRender final
{
    RA_SINGLETON_INTERFACE( DebugRender );

  public:
    DebugRender();
    ~DebugRender();

    void initialize();
    void render( const Core::Matrix4& view, const Core::Matrix4& proj );

    void
    addLine( const Core::Vector3& from, const Core::Vector3& to, const Core::Utils::Color& color );
    void addPoint( const Core::Vector3& p, const Core::Utils::Color& color );
    void addPoints( const Core::Vector3Array& p, const Core::Utils::Color& color );
    void addPoints( const Core::Vector3Array& p, const Core::Vector4Array& colors );
    void addMesh( const std::shared_ptr<Mesh>& mesh,
                  const Core::Transform& transform = Core::Transform::Identity() );

    // Shortcuts
    void addCross( const Core::Vector3& position, Scalar size, const Core::Utils::Color& color );

    void addSphere( const Core::Vector3& center, Scalar radius, const Core::Utils::Color& color );

    void addCircle( const Core::Vector3& center,
                    const Core::Vector3& normal,
                    Scalar radius,
                    const Core::Utils::Color& color );

    void addFrame( const Core::Transform& transform, Scalar size );

    void addTriangle( const Core::Vector3& p0,
                      const Core::Vector3& p1,
                      const Core::Vector3& p2,
                      const Core::Utils::Color& color );

    void addAABB( const Core::Aabb& box, const Core::Utils::Color& color );

    void addOBB( const Core::Aabb& box,
                 const Core::Transform& transform,
                 const Core::Utils::Color& color );

  private:
    struct Line {
        Line( Core::Vector3 la,  Core::Vector3 lb,  Core::Utils::Color lcol ) :
            a{std::move(la)},
            b{std::move(lb)},
            col{std::move(lcol)} {}

        Core::Vector3 a, b;
        Core::Utils::Color col;
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
    uint m_lineProg {0};
    uint m_pointProg {0};
    uint m_meshProg {0};

    uint m_modelLineLoc {0};
    uint m_viewLineLoc {0};
    uint m_projLineLoc {0};

    uint m_modelMeshLoc {0};
    uint m_viewMeshLoc {0};
    uint m_projMeshLoc {0};

    uint m_viewPointLoc {0};
    uint m_projPointLoc {0};

    std::vector<Line> m_lines;
    std::vector<DbgMesh> m_meshes;

    std::vector<Point> m_points;
};

} // namespace Ra::Engine

#endif // RADIUMENGINE_DEBUGRENDERER_HPP
