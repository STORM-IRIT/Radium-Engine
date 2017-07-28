#ifndef RADIUMENGINE_POINTCLOUDCOMPONENT_HPP
#define RADIUMENGINE_POINTCLOUDCOMPONENT_HPP

#include <PointCloudPluginMacros.hpp>

#include <Core/Mesh/MeshTypes.hpp>
#include <Core/Mesh/TriangleMesh.hpp>

#include <Engine/Component/Component.hpp>
#include <Core/File/HandleData.hpp>

namespace Ra
{
    namespace Engine
    {
        struct RenderTechnique;
        class Mesh;
    }

    namespace Asset
    {
        class GeometryData;
    }
}

namespace PointCloudPlugin
{
    /*!
     * \brief The PointCloudComponent class
     *
     * Exports access to the mesh geometry:
     *  - TriangleMesh: get, rw (set vertices, normals and triangles dirty)
     *  - Vertices: rw (if deformable)
     *  - normals: rw (if deformable)
     *  - triangles: rw (if deformable)
     */
    class PCLOUD_PLUGIN_API PointCloudComponent : public Ra::Engine::Component
{
    public:
        PointCloudComponent( const std::string& name);
        virtual ~PointCloudComponent();
        virtual void initialize() override;

     /*   void handleDataLoading( const Ra::Asset::GeometryData* data);
        void setupIO(const std::string &id);
        void setUseNormal(bool useNormal);
        const bool* getUseNormal() const;
    private:
        std::string m_contentName;
        bool m_useNormal;
   */     
};

} // namespace PointCloudPlugin


#endif //RADIUMENGINE_POINTCLOUDCOMPONENT_HPP
