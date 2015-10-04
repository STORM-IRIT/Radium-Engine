#ifndef ANIMPLUGIN_SKELETON_BONE_DRAWABLE_HPP_ 
#define ANIMPLUGIN_SKELETON_BONE_DRAWABLE_HPP_  

#include <Plugins/Animation/AnimationPlugin.hpp>

#include <Core/Mesh/MeshUtils.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Plugins/Animation/Skeleton/Skeleton.hpp>
#include <Plugins/Animation/AnimationComponent.hpp>

namespace AnimationPlugin
{
class SkeletonBoneRenderObject : public Ra::Engine::RenderObject
{
    SkeletonBoneRenderObject(const std::string& name, const AnimationComponent* comp, uint boneIdx)
        : RenderObject(name, comp),
        m_skel( comp->getSkeleton()), m_boneIdx(boneIdx)
    {}
    
    static Ra::Core::TriangleMesh makeBoneShape()
    {
        // Bone along Z axis.

        Ra::Core::TriangleMesh mesh;

        const Scalar l = 0.2f;
        const Scalar w = 0.2f;

        mesh.m_vertices = {
            Ra::Core::Vector3(0,0,0), Ra::Core::Vector3(0,0,1),
            Ra::Core::Vector3(0,w,l), Ra::Core::Vector3(w,0,l),
            Ra::Core::Vector3(0,-w,l), Ra::Core::Vector3(-w,0,l)
        };

        mesh.m_triangles = {
            Ra::Core::Triangle(0,3,2),Ra::Core::Triangle(0,5,3),
            Ra::Core::Triangle(0,2,4),Ra::Core::Triangle(0,4,5),
            Ra::Core::Triangle(1,2,3),Ra::Core::Triangle(1,3,5),
            Ra::Core::Triangle(1,4,2),Ra::Core::Triangle(1,5,4)
        };
        Ra::Core::MeshUtils::getAutoNormals(mesh, mesh.m_normals);
        return mesh;
    }




    protected:
        const Skeleton& m_skel;
        uint m_boneIdx;
};

}

#endif //ANIMPLUGIN_SKELETON_BONE_DRAWABLE_HPP_ 
