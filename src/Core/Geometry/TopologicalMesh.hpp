#ifndef TOPOLOGICALMESH_H
#define TOPOLOGICALMESH_H

#include <Core/Geometry/OpenMesh.h>

#include <Core/Geometry/TriangleMesh.hpp>
#include <Core/RaCore.hpp>
#include <Core/Types.hpp>
#include <Core/Utils/Index.hpp>

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/Traits.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Utils/Property.hh>
#include <OpenMesh/Core/Utils/PropertyManager.hh>

#include <Eigen/Core>
#include <Eigen/Geometry>

namespace Ra {
namespace Core {
namespace Geometry {

/**
 * Define the Traits to be used by OpenMesh for TopologicalMesh.
 */
struct TopologicalMeshTraits : OpenMesh::DefaultTraits {
    using Point  = Ra::Core::Vector3;
    using Normal = Ra::Core::Vector3;

    VertexAttributes( OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal );
    FaceAttributes( OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal );
    EdgeAttributes( OpenMesh::Attributes::Status );
    HalfedgeAttributes( OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal );
};

/**
 * This class represents a mesh with topological information on the
 * vertex graph, using a half-edge representation.
 *
 * This integration is inspired by: https://gist.github.com/Unril/03fa353d0461ed6bd41d
 */
class RA_CORE_API TopologicalMesh : public OpenMesh::PolyMesh_ArrayKernelT<TopologicalMeshTraits>
{
  private:
    using base = OpenMesh::PolyMesh_ArrayKernelT<TopologicalMeshTraits>;
    using base::PolyMesh_ArrayKernelT;
    using Index = Ra::Core::Utils::Index;

    OpenMesh::HPropHandleT<Index> m_inputTriangleMeshIndexPph;
    OpenMesh::HPropHandleT<Index> m_outputTriangleMeshIndexPph;
    std::vector<OpenMesh::HPropHandleT<float>> m_floatPph;
    std::vector<OpenMesh::HPropHandleT<Vector2>> m_vec2Pph;
    std::vector<OpenMesh::HPropHandleT<Vector3>> m_vec3Pph;
    std::vector<OpenMesh::HPropHandleT<Vector4>> m_vec4Pph;

    friend class TMOperations;

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /**
     * Construct a topological mesh from a triangle mesh.
     * This operation merges vertex with same position, but keeps vertex
     * attributes on halfedges, so that TriangleMesh vertices with the same 3D
     * position are represented only once in the topological mesh.
     * \note This is a costly operation.
     */
    explicit TopologicalMesh( const Ra::Core::Geometry::TriangleMesh& triMesh );

    /**
     * Construct an empty topological mesh
     */
    explicit TopologicalMesh() {}

    /**
     * Return a triangleMesh from the topological mesh.
     * \note This is a costly operation.
     * \warning It uses the attributes defined on halfedges.
     */
    TriangleMesh toTriangleMesh();

    /**
     * Update triangle mesh data, assuming the mesh and this topo mesh has the
     * same topology.
     * \warning note implemented yet.
     */
    void updateTriangleMesh( Ra::Core::Geometry::TriangleMesh& mesh );

    // import other version of halfedge_handle method
    using base::halfedge_handle;

    /**
     * Return the half-edge associated with a given vertex and face.
     * \note Asserts if vh is not a member of fh.
     */
    inline HalfedgeHandle halfedge_handle( VertexHandle vh, FaceHandle fh ) const;

    /**
     * Get normal of the vertex vh, when member of fh.
     * \note Asserts if vh is not a member of fh.
     */
    inline const Normal& normal( VertexHandle vh, FaceHandle fh ) const;

    /**
     * Set normal of the vertex vh, when member of fh.
     * \note Asserts if vh is not a member of fh.
     */
    void set_normal( VertexHandle vh, FaceHandle fh, const Normal& n );

    /// Import Base definition of normal and set normal.
    ///@{
    using base::normal;
    using base::set_normal;
    ///@}

    /**
     * Set the normal n to all the halfedges that points to vh (i.e. incomming
     * halfedges) .
     * If you work with vertex normals, please call this function on all vertex
     * handles before convertion with toTriangleMesh.
     */
    void propagate_normal_to_halfedges( VertexHandle vh );

    /**
     * Return a handle to the halfedge property storing vertices indices within
     * the TriangleMesh *this has been built on.
     */
    inline const OpenMesh::HPropHandleT<Index>& getInputTriangleMeshIndexPropHandle() const;

    /**
     * Return a handle to the halfedge property storing vertices indices within
     * the TriangleMesh returned by toTriangleMesh().
     * \note This property is valid only after toTriangleMesh() has been called.
     */
    inline const OpenMesh::HPropHandleT<Index>& getOutputTriangleMeshIndexPropHandle() const;

    /**
     * \name Const access to handles of the HalfEdge properties coming from
     * the TriangleMesh attributes.
     */
    ///@{
    inline const std::vector<OpenMesh::HPropHandleT<float>>& getFloatPropsHandles() const;
    inline const std::vector<OpenMesh::HPropHandleT<Vector2>>& getVector2PropsHandles() const;
    inline const std::vector<OpenMesh::HPropHandleT<Vector3>>& getVector3PropsHandles() const;
    inline const std::vector<OpenMesh::HPropHandleT<Vector4>>& getVector4PropsHandles() const;
    ///@}

    /**
     * \name Dealing with normals
     * Utils to deal with normals when modifying the mesh topology.
     */
    ///@{

    /**
     * Create a new property for normals on faces of \p mesh.
     * \note This new property will have to be propagated onto the newly created
     * halfedges with copyNormal().
     */
    inline void createNormalPropOnFaces( OpenMesh::FPropHandleT<Normal>& fProp );

    /**
     * Remove face property \p prop from \p mesh.
     * \note Invalidates the property handle.
     */
    inline void clearProp( OpenMesh::FPropHandleT<Normal>& fProp );

    /**
     * Copy the normal property from \p input_heh to \p copy_heh.
     */
    inline void copyNormal( HalfedgeHandle input_heh, HalfedgeHandle copy_heh );

    /** Copy the face normal property \p fProp from \p fh to \p heh.
     * \note \p fProp must have been previously created through createNormalPropOnFaces().
     */
    inline void
    copyNormalFromFace( FaceHandle fh, HalfedgeHandle heh, OpenMesh::FPropHandleT<Normal> fProp );

    /**
     * Interpolate normal property on edge center (after edge split).
     */
    inline void
    interpolateNormal( HalfedgeHandle in_a, HalfedgeHandle in_b, HalfedgeHandle out, Scalar f );

    /** Interpolate normal property on face center.
     * \note \p fProp must have been previously created through createNormalPropOnFaces().
     */
    inline void interpolateNormalOnFaces( FaceHandle fh, OpenMesh::FPropHandleT<Normal> fProp );
    ///@}

    /**
     * \name Dealing with custom properties
     * Utils to deal with custom properties of any type when modifying the mesh topology.
     */
    ///@{

    /**
     * Create a new property for each \p input properties of \p mesh on faces.
     * \note This new property will have to be propagated onto the newly created
     * halfedges with copyProps().
     */
    template <typename T>
    void createPropsOnFaces( const std::vector<OpenMesh::HPropHandleT<T>>& input,
                             std::vector<OpenMesh::FPropHandleT<T>>& output );

    /**
     * Remove \p props from \p mesh.
     * \note Clears \p props.
     */
    template <typename T>
    void clearProps( std::vector<OpenMesh::FPropHandleT<T>>& props );

    /**
     * Copy \p props properties from \p input_heh to \p copy_heh.
     */
    template <typename T>
    void copyProps( HalfedgeHandle input_heh,
                    HalfedgeHandle copy_heh,
                    const std::vector<OpenMesh::HPropHandleT<T>>& props );

    /**
     * Copy face properties \p props from \p fh to \p heh.
     * \note \p fProps must have been previously created through createPropsOnFaces().
     */
    template <typename T>
    void copyPropsFromFace( FaceHandle fh,
                            HalfedgeHandle heh,
                            const std::vector<OpenMesh::FPropHandleT<T>>& fProps,
                            const std::vector<OpenMesh::HPropHandleT<T>>& hProps );

    /**
     * Interpolate \p props on edge center (after edge split).
     */
    template <typename T>
    void interpolateProps( HalfedgeHandle in_a,
                           HalfedgeHandle in_b,
                           HalfedgeHandle out,
                           Scalar f,
                           const std::vector<OpenMesh::HPropHandleT<T>>& props );

    /**
     * Interpolate \p hprops on face center.
     * \note \p fProps must have been previously created through createPropsOnFaces().
     */
    template <typename T>
    void interpolatePropsOnFaces( FaceHandle fh,
                                  const std::vector<OpenMesh::HPropHandleT<T>>& hProps,
                                  const std::vector<OpenMesh::FPropHandleT<T>>& fProps );
    ///@}

    /**
     * \name Deal with all attributes
     * Utils to deal with the normal and custom properties when modifying the mesh topology.
     */
    ///@{

    /**
     * Create a new property for each property of \p mesh on faces.
     * Outputs the new face properties handles in the corresponding output parameters.
     * \note These new properties will have to be propagated onto the newly created
     * halfedges with copyAllProps().
     */
    inline void createAllPropsOnFaces( OpenMesh::FPropHandleT<Normal>& normalProp,
                                       std::vector<OpenMesh::FPropHandleT<Scalar>>& floatProps,
                                       std::vector<OpenMesh::FPropHandleT<Vector2>>& vec2Props,
                                       std::vector<OpenMesh::FPropHandleT<Vector3>>& vec3Props,
                                       std::vector<OpenMesh::FPropHandleT<Vector4>>& vec4Props );

    /**
     * Remove all the given properties from \p mesh.
     * \note Invalidates \p normalProp and clears the given property containers.
     */
    inline void clearAllProps( OpenMesh::FPropHandleT<Normal>& normalProp,
                               std::vector<OpenMesh::FPropHandleT<Scalar>>& floatProps,
                               std::vector<OpenMesh::FPropHandleT<Vector2>>& vec2Props,
                               std::vector<OpenMesh::FPropHandleT<Vector3>>& vec3Props,
                               std::vector<OpenMesh::FPropHandleT<Vector4>>& vec4Props );

    /**
     * Copy all properties from \p input_heh to \p copy_heh.
     */
    inline void copyAllProps( HalfedgeHandle input_heh, HalfedgeHandle copy_heh );

    /**
     * Copy all given face properties from \p fh to \p heh.
     * \note Each property must have been previously created either all at once
     * through createAllPropsOnFaces(), or individually through
     * createNormalPropOnFaces() and createPropsOnFaces().
     */
    inline void copyAllPropsFromFace( FaceHandle fh,
                                      HalfedgeHandle heh,
                                      OpenMesh::FPropHandleT<Normal> normalProp,
                                      std::vector<OpenMesh::FPropHandleT<Scalar>>& floatProps,
                                      std::vector<OpenMesh::FPropHandleT<Vector2>>& vec2Props,
                                      std::vector<OpenMesh::FPropHandleT<Vector3>>& vec3Props,
                                      std::vector<OpenMesh::FPropHandleT<Vector4>>& vec4Props );

    /**
     * Interpolate all properties on edge center (after edge split).
     */
    inline void
    interpolateAllProps( HalfedgeHandle in_a, HalfedgeHandle in_b, HalfedgeHandle out, Scalar f );

    /**
     * Interpolate \p hprops on face center.
     * \note Each property must have been previously created either all at once
     * through createAllPropsOnFaces(), or individually through
     * createNormalPropOnFaces() and createPropsOnFaces().
     */
    inline void
    interpolateAllPropsOnFaces( FaceHandle fh,
                                OpenMesh::FPropHandleT<Normal> normalProp,
                                std::vector<OpenMesh::FPropHandleT<Scalar>>& floatProps,
                                std::vector<OpenMesh::FPropHandleT<Vector2>>& vec2Props,
                                std::vector<OpenMesh::FPropHandleT<Vector3>>& vec3Props,
                                std::vector<OpenMesh::FPropHandleT<Vector4>>& vec4Props );
    ///@}

    /**
     * \name Topological operations
     */
    ///@{
    /**
     * \brief Apply a 2-4 edge split.
     * \param eh The handle to the edge to split.
     * \param f The interpolation factor to place the new point on the edge.
     *          Must be in [0,1].
     * \return True if the edge has been split, false otherwise.
     * \note Only applies on edges between 3 triangles, and if \p f is in [0,1].
     * \note Mesh attributes are linearly interpolated on the newly created halfedge.
     */
    bool splitEdge( TopologicalMesh::EdgeHandle eh, Scalar f );
    ///@}
};

} // namespace Geometry
} // namespace Core
} // namespace Ra

#include <Core/Geometry/TopologicalMesh.inl>

#endif // TOPOLOGICALMESH_H
