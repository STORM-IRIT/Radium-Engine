#pragma once

#include <Core/RaCore.hpp>

#include <Core/Containers/VectorArray.hpp>
#include <Core/Geometry/OpenMesh.hpp>
#include <Core/Geometry/TriangleMesh.hpp>
#include <Core/Types.hpp>
#include <Core/Utils/Index.hpp>
#include <Core/Utils/StdOptional.hpp>

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/Traits.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Utils/Property.hh>
#include <OpenMesh/Core/Utils/PropertyManager.hh>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <set>

namespace Ra {
namespace Core {
namespace Geometry {
namespace deprecated {
using namespace Utils; // log, AttribXXX

/**
 * Define the Traits to be used by OpenMesh for TopologicalMesh.
 */
struct TopologicalMeshTraits : OpenMesh::DefaultTraits {
    using Point  = Ra::Core::Vector3;
    using Normal = Ra::Core::Vector3;

    VertexAttributes( OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal );
    FaceAttributes( OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal );
    EdgeAttributes( OpenMesh::Attributes::Status );
    // Add  OpenMesh::Attributes::PrevHalfedge for efficiency ?
    HalfedgeAttributes( OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal );
};

/**
 * This class represents a mesh with topological information on the
 * vertex graph, using a half-edge representation.
 *
 * This integration is inspired by:
 * https://gist.github.com/Unril/03fa353d0461ed6bd41d
 *
 * \todo rename methods to respect Radium guideline (get/set/is, camelCase)
 * \todo private inheritance from OpenMesh, and import relevant methods.
 */
class RA_CORE_API TopologicalMesh : public OpenMesh::PolyMesh_ArrayKernelT<TopologicalMeshTraits>
{
  private:
    using base = OpenMesh::PolyMesh_ArrayKernelT<TopologicalMeshTraits>;
    using base::PolyMesh_ArrayKernelT;
    using Index   = Ra::Core::Utils::Index;
    using Vector3 = Ra::Core::Vector3;

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /**
     * Construct a topological mesh from a triangle mesh.
     * This operation merges vertex with same position, but keeps vertex
     * attributes on halfedges, so that TriangleMesh vertices with the same 3D
     * position are represented only once in the topological mesh.
     * \note This is a costly operation.
     *
     * \tparam NonManifoldFaceCommand Command executed when non-manifold faces are
     * found. API and default implementation:
     * \snippet Core/Geometry/TopologicalMesh.cpp Default command implementation
     *
     */
    template <typename NonManifoldFaceCommand>
    explicit TopologicalMesh( const Ra::Core::Geometry::TriangleMesh& triMesh,
                              NonManifoldFaceCommand command );

    /**
     * \brief Convenience constructor
     * \see TopologicalMesh( const Ra::Core::Geometry::TriangleMesh&, NonManifoldFaceCommand)
     */
    explicit TopologicalMesh( const Ra::Core::Geometry::TriangleMesh& triMesh );

    /**
     * Construct an empty topological mesh, only init mandatory properties.
     */
    explicit TopologicalMesh();

    /**
     * Return a triangleMesh from the topological mesh.
     * \note This is a costly operation.
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
    [[deprecated]] inline const Normal& normal( VertexHandle vh, FaceHandle fh ) const;

    /**
     * Set normal of the vertex vh, when member of fh.
     * \note Asserts if vh is not a member of fh.
     */
    [[deprecated]] void set_normal( VertexHandle vh, FaceHandle fh, const Normal& n );

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
    [[deprecated]] void propagate_normal_to_halfedges( VertexHandle vh );

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
    [[deprecated]] inline const std::vector<OpenMesh::HPropHandleT<Scalar>>&
    getFloatPropsHandles() const;
    [[deprecated]] inline const std::vector<OpenMesh::HPropHandleT<Vector2>>&
    getVector2PropsHandles() const;
    [[deprecated]] inline const std::vector<OpenMesh::HPropHandleT<Vector3>>&
    getVector3PropsHandles() const;
    [[deprecated]] inline const std::vector<OpenMesh::HPropHandleT<Vector4>>&
    getVector4PropsHandles() const;
    ///@}

    /**
     * \name Dealing with normals
     * Utils to deal with normals when modifying the mesh topology.
     */
    ///@{

    /**
     * Create a new property for normals on faces of \a mesh.
     * \note This new property will have to be propagated onto the newly created
     * halfedges with copyNormal().
     */
    inline void createNormalPropOnFaces( OpenMesh::FPropHandleT<Normal>& fProp );

    /**
     * Remove face property \a prop from \a mesh.
     * \note Invalidates the property handle.
     */
    inline void clearProp( OpenMesh::FPropHandleT<Normal>& fProp );

    /**
     * Copy the normal property from \a input_heh to \a copy_heh.
     */
    inline void copyNormal( HalfedgeHandle input_heh, HalfedgeHandle copy_heh );

    /** Copy the face normal property \a fProp from \a fh to \a heh.
     * \note \a fProp must have been previously created through createNormalPropOnFaces().
     */
    inline void
    copyNormalFromFace( FaceHandle fh, HalfedgeHandle heh, OpenMesh::FPropHandleT<Normal> fProp );

    /**
     * Interpolate normal property on edge center (after edge split).
     */
    inline void
    interpolateNormal( HalfedgeHandle in_a, HalfedgeHandle in_b, HalfedgeHandle out, Scalar f );

    /** Interpolate normal property on face center.
     * \note \a fProp must have been previously created through createNormalPropOnFaces().
     */
    inline void interpolateNormalOnFaces( FaceHandle fh, OpenMesh::FPropHandleT<Normal> fProp );
    ///@}

    /**
     * \name Dealing with custom properties
     * Utils to deal with custom properties of any type when modifying the mesh topology.
     */
    ///@{

    /**
     * Create a new property for each \a input properties of \a mesh on faces.
     * \note This new property will have to be propagated onto the newly created
     * halfedges with copyProps().
     */
    template <typename T>
    void createPropsOnFaces( const std::vector<OpenMesh::HPropHandleT<T>>& input,
                             std::vector<OpenMesh::FPropHandleT<T>>& output );

    /**
     * Remove \a props from \a mesh.
     * \note Clears \a props.
     */
    template <typename T>
    void clearProps( std::vector<OpenMesh::FPropHandleT<T>>& props );

    /**
     * Copy \a props properties from \a input_heh to \a copy_heh.
     */
    template <typename T>
    void copyProps( HalfedgeHandle input_heh,
                    HalfedgeHandle copy_heh,
                    const std::vector<OpenMesh::HPropHandleT<T>>& props );

    /**
     * Copy face properties \a props from \a fh to \a heh.
     * \note \a fProps must have been previously created through createPropsOnFaces().
     */
    template <typename T>
    void copyPropsFromFace( FaceHandle fh,
                            HalfedgeHandle heh,
                            const std::vector<OpenMesh::FPropHandleT<T>>& fProps,
                            const std::vector<OpenMesh::HPropHandleT<T>>& hProps );

    /**
     * Interpolate \a props on edge center (after edge split).
     */
    template <typename T>
    void interpolateProps( HalfedgeHandle in_a,
                           HalfedgeHandle in_b,
                           HalfedgeHandle out,
                           Scalar f,
                           const std::vector<OpenMesh::HPropHandleT<T>>& props );

    /**
     * Interpolate \a hprops on face center.
     * \note \a fProps must have been previously created through createPropsOnFaces().
     */
    template <typename T>
    void interpolatePropsOnFaces( FaceHandle fh,
                                  const std::vector<OpenMesh::HPropHandleT<T>>& hProps,
                                  const std::vector<OpenMesh::FPropHandleT<T>>& fProps );
    ///@}
    /**
        * \name Deal with all attributes* Utils to deal with the normal and
        custom properties when modifying the mesh topology.*/

    ///@{

    /**
     * Create a new property for each property of \a mesh on faces.
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
     * Remove all the given properties from \a mesh.
     * \note Invalidates \a normalProp and clears the given property containers.
     */
    inline void clearAllProps( OpenMesh::FPropHandleT<Normal>& normalProp,
                               std::vector<OpenMesh::FPropHandleT<Scalar>>& floatProps,
                               std::vector<OpenMesh::FPropHandleT<Vector2>>& vec2Props,
                               std::vector<OpenMesh::FPropHandleT<Vector3>>& vec3Props,
                               std::vector<OpenMesh::FPropHandleT<Vector4>>& vec4Props );

    /**
     * Copy all properties from \a input_heh to \a copy_heh.
     */
    inline void copyAllProps( HalfedgeHandle input_heh, HalfedgeHandle copy_heh );

    /**
     * Copy all given face properties from \a fh to \a heh.
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
     * Interpolate \a hprops on face center.
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
     * Apply a 2-4 edge split.
     * \param eh The handle to the edge to split.
     * \param f The interpolation factor to place the new point on the edge.
     *          Must be in [0,1].
     * \return True if the edge has been split, false otherwise.
     * \note Only applies on edges between 2 triangles, and if \a f is in [0,1].
     * \note Mesh attributes are linearly interpolated on the newly created
     * halfedge.
     * \note f=0 correspond to halfedge_handle( eh, 0 ) (i.e. first vertex of
     * the edge)
     */
    bool splitEdge( TopologicalMesh::EdgeHandle eh, Scalar f );
    bool splitEdgeWedge( TopologicalMesh::EdgeHandle eh, Scalar f );

    ///@}

  private:
    template <typename T>
    using HandleAndValueVector =
        std::vector<std::pair<AttribHandle<T>, T>,
                    Eigen::aligned_allocator<std::pair<AttribHandle<T>, T>>>;

    template <typename T>
    using PropPair = std::pair<AttribHandle<T>, OpenMesh::HPropHandleT<T>>;

    template <typename T>
    inline void copyAttribToTopo( const TriangleMesh& triMesh,
                                  const std::vector<PropPair<T>>& vprop,
                                  TopologicalMesh::HalfedgeHandle heh,
                                  unsigned int vindex );

    template <typename T>
    inline void addAttribPairToTopo( const TriangleMesh& triMesh,
                                     AttribManager::pointer_type attr,
                                     std::vector<PropPair<T>>& vprop,
                                     std::vector<OpenMesh::HPropHandleT<T>>& pph );

    void split_copy( EdgeHandle _eh, VertexHandle _vh );
    void split( EdgeHandle _eh, VertexHandle _vh );

    ///\todo to be deleted/updated
    OpenMesh::HPropHandleT<Index> m_inputTriangleMeshIndexPph;
    OpenMesh::HPropHandleT<Index> m_outputTriangleMeshIndexPph;
    [[deprecated]] std::vector<OpenMesh::HPropHandleT<Scalar>> m_floatPph;
    [[deprecated]] std::vector<OpenMesh::HPropHandleT<Vector2>> m_vec2Pph;
    [[deprecated]] std::vector<OpenMesh::HPropHandleT<Vector3>> m_vec3Pph;
    [[deprecated]] std::vector<OpenMesh::HPropHandleT<Vector4>> m_vec4Pph;

    friend class TMOperations;
};

} // namespace deprecated
} // namespace Geometry
} // namespace Core
} // namespace Ra
#include <Core/Geometry/deprecated/TopologicalMesh.inl>
