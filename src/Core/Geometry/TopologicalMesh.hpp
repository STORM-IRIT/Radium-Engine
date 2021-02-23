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
    class Wedge;

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    using WedgeIndex = Index;

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
    void initWithWedge( const Ra::Core::Geometry::TriangleMesh& triMesh );
    template <typename NonManifoldFaceCommand>
    void initWithWedge( const Ra::Core::Geometry::TriangleMesh& triMesh,
                        NonManifoldFaceCommand command );

    /**
     * Construct an empty topological mesh, only init mandatory properties.
     */
    explicit TopologicalMesh();

    /**
     * Return a triangleMesh from the topological mesh.
     * \note This is a costly operation.
     * \warning It uses the attributes defined on halfedges. Do not work well if attribs are defined
     * on wedges (initWithWedge), in this case use toTriangleMeshFromWedges()
     */
    TriangleMesh toTriangleMesh();

    /**
     * Return a triangleMesh from the topological mesh.
     * \note This is a costly operation.
     * \warning It uses the attributes defined on wedges
     */
    TriangleMesh toTriangleMeshFromWedges();

    /**
     * Update triangle mesh data, assuming the mesh and this topo mesh has the
     * same topology.
     */
    void updateTriangleMesh( Ra::Core::Geometry::TriangleMesh& mesh );
    void update( const Ra::Core::Geometry::TriangleMesh& mesh );

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
    [[deprecated]] inline const std::vector<OpenMesh::HPropHandleT<float>>&
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

    void updateWedgeNormals();
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
     * Inner class WedgeData represents the actual data per wedge, including position.
     *
     * At any time m_position as to be equal to the wedge's vertex point.
     * All wedges have the same set of attributes.
     * Access and management is delegated to TopologicalMesh and WedgeCollection
     */
    class WedgeData
    {
      public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        //        Index m_inputTriangleMeshIndex;
        //        Index m_outputTriangleMeshIndex;
        Vector3 m_position {};
        VectorArray<Scalar> m_floatAttrib;
        VectorArray<Vector2> m_vector2Attrib;
        VectorArray<Vector3> m_vector3Attrib;
        VectorArray<Vector4> m_vector4Attrib;

        template <typename T>
        inline VectorArray<T>& getAttribArray();

        explicit WedgeData() = default;
        inline bool operator==( const WedgeData& lhs ) const;
        inline bool operator!=( const WedgeData& lhs ) const;
        inline bool operator<( const WedgeData& lhs ) const;
        friend Wedge;

      private:
        // return 1 : equals, 2: strict less, 3: strict greater
        template <typename T>
        static int compareVector( const T& a, const T& b );
    };

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

    /**
     * Halfedge collapes \a he.
     * vo=from_vertex_handle(he) is deleted.
     * After collapse vo incoming halfedges points to vh = to_vertex_handle(he).
     * Wedge indices are updated to reflect the change in topology.
     * For detailed topological modifications see \ref develmeshes.
     * \param he halfedge's hangle to collapse.
     */
    void collapseWedge( TopologicalMesh::HalfedgeHandle he );
    ///@}

    /**
     * Return the set of WedgeIndex incident to a given Vertex \a vh.
     * only valid non deleted wedges are present in the set.
     */
    inline std::set<WedgeIndex> getVertexWedges( OpenMesh::VertexHandle vh ) const;

    /**
     * get the wedge index associated with an halfedge
     */
    inline WedgeIndex getWedgeIndex( OpenMesh::HalfedgeHandle heh ) const;

    /**
     * Access to wedge data.
     * \param idx must be valid and correspond to a non delete wedge index.
     */
    inline const WedgeData& getWedgeData( const WedgeIndex& idx ) const;

    /**
     * Return the wedge refcount, for debug purpose.
     */
    inline unsigned int getWedgeRefCount( const WedgeIndex& idx ) const;

    /** set WedgeData \a wd to the wedge with index \a widx.
     * All halfedge that point to widx will get the new values.
     * \param widx index of the wedge
     * \param wd data to set to wedge that correspond to widx
     */
    inline void setWedgeData( WedgeIndex widx, const WedgeData& wd );

    /**
     * Change the WedgeData associated for \a idx, for attrib \a name to \a value.
     * The data is changed for all halfedges referencing this wedge.
     * \return true if the wedge is set, false if nothing set, i.e. if name is not an attrib of
     * type T.
     */
    template <typename T>
    inline bool setWedgeData( const WedgeIndex& idx, const std::string& name, const T& value );

    /**
     * Replace the wedge data associated with an halfedge.
     * The old wedge is "deleted". If wedge data correspond to an already
     * present wedge, it's index is used.
     */
    inline void replaceWedge( OpenMesh::HalfedgeHandle he, const WedgeData& wd );

    /**
     * Replace the wedge index associated with an halfedge.
     * The old wedge is "deleted". The new wedge reference count is incremented.
     */
    inline void replaceWedgeIndex( OpenMesh::HalfedgeHandle he, const WedgeIndex& widx );

    /**
     * call mergeEquelWedges( vh ) for every vertices of the mesh.
     * \see void mergeEqualWedges( OpenMesh::VertexHandle vh );
     */
    inline void mergeEqualWedges();

    /**
     * Merge (make same index) wegdes with the same data around \a vh
     * \param vh vertex handle to process
     */
    inline void mergeEqualWedges( OpenMesh::VertexHandle vh );

    /// Remove deleted element from the mesh, including wedges.
    void garbage_collection();

    inline const std::vector<std::string>& getVec4AttribNames() const;
    inline const std::vector<std::string>& getVec3AttribNames() const;
    inline const std::vector<std::string>& getVec2AttribNames() const;
    inline const std::vector<std::string>& getFloatAttribNames() const;

    /// true if more than one wedge arount vertex \a vh, false if only one wedge
    inline bool isFeatureVertex( const VertexHandle& vh ) const;

    /// true if at least one of edge's vertex as two different wedge arount the
    /// edge.
    /// false if the two vertices have the same wedge for both face aside the edge.
    inline bool isFeatureEdge( const EdgeHandle& eh ) const;

    inline const OpenMesh::HPropHandleT<WedgeIndex>& getWedgeIndexPph() const;

    void delete_face( FaceHandle _fh, bool _delete_isolated_vertices = true );

    /**
     * is the vertex a "bow tie" vertex ?
     * \note Alias for OpenMesh::is_manifold
     */
    bool isManifold( VertexHandle vh ) const;

    /// Check if evrything looks right in the data structure
    /// \return true if ok, false if ko.
    bool checkIntegrity() const;

  private:
    // internal function to build TriangleMesh attribs correspondance to wedge attribs.
    class RA_CORE_API InitWedgeProps
    {
      public:
        InitWedgeProps( TopologicalMesh* topo, const TriangleMesh& triMesh ) :
            m_topo( topo ), m_triMesh( triMesh ) {}
        void operator()( AttribBase* attr ) const;

      private:
        TopologicalMesh* m_topo;
        const TriangleMesh& m_triMesh;
    };

    class WedgeCollection;
    //
    /**
     * This private class manage wedge data and refcount, to maintain deleted status
     *
     * \internal We need to export this class to make it accessible in .inl
     */
    class RA_CORE_API Wedge
    {
        WedgeData m_wedgeData {};
        unsigned int m_refCount {0};

      private:
        WedgeData& getWedgeData() { return m_wedgeData; }

      public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        explicit Wedge() {}
        explicit Wedge( const WedgeData& wd ) : m_wedgeData {wd}, m_refCount {1} {};
        const WedgeData& getWedgeData() const { return m_wedgeData; }
        void setWedgeData( const WedgeData& wedgeData ) { m_wedgeData = wedgeData; }
        void setWedgeData( WedgeData&& wedgeData ) { m_wedgeData = std::move( wedgeData ); }
        void incrementRefCount() { ++m_refCount; }
        void decrementRefCount() {
            if ( m_refCount ) --m_refCount;
        }
        /// comparison ignore refCount
        bool operator==( const Wedge& lhs ) const { return m_wedgeData == lhs.m_wedgeData; }

        bool isDeleted() const { return m_refCount == 0; }
        unsigned int getRefCount() const { return m_refCount; }

        friend WedgeCollection;
    };

    /**
     * This private class manage the wedge collection.
     * Most of the data members are public so that the enclosing class can
     * easily manage the data.
     *
     * \internal We need to export this class to make it accessible in .inl
     */
    class RA_CORE_API WedgeCollection
    {
      public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        template <typename T>
        inline const std::vector<std::string>& getNameArray() const;

        template <typename T>
        inline std::vector<std::string>& getNameArray();

        /**
         * Add wd to the wedge collection, and return the index.
         * If a wedge with same data is already present, it's index is returned,
         * otherwise a new wedge is added to the wedges collection.
         * \param wd Data to insert.
         * \return the index of the inserted (or found) wedge.
         */
        WedgeIndex add( const WedgeData& wd );

        /**
         * Delete the wedge \a idx from the collection.
         * These deletion actually just remove one reference from an halfedge
         * to the wedge data. If the wedge is still referenced by other
         * halfedges, it will not be removed during garbageCollection.
         */
        void del( const WedgeIndex& idx );
        WedgeIndex newReference( const WedgeIndex& idx );

        /**
         * Return the wedge data associated with \a idx
         */
        const WedgeData& getWedgeData( const WedgeIndex& idx ) const {
            CORE_ASSERT( idx.isValid() && !m_data[idx].isDeleted(),
                         "access to invalid or deleted wedge is prohibited" );

            return m_data[idx].getWedgeData();
        }

        unsigned int getWedgeRefCount( const WedgeIndex& idx ) const {
            CORE_ASSERT( idx.isValid(), "access to invalid or deleted wedge is prohibited" );
            return m_data[idx].getRefCount();
        }

        /// Return the wedge (not the data) for in class manipulation.
        /// client code should use getWedgeData only.
        inline const Wedge& getWedge( const WedgeIndex& idx ) const;

        /// \see TopologicalMesh::setWedgeData
        inline void setWedgeData( const WedgeIndex& idx, const WedgeData& wd );

        /// \see TopologicalMesh::setWedgeData<T>
        template <typename T>
        inline bool setWedgeData( const WedgeIndex& idx, const std::string& name, const T& value );
        inline bool setWedgePosition( const WedgeIndex& idx, const Vector3& value );

        // name is supposed to be unique within all attribs
        // not checks are performed
        template <typename T>
        void addProp( const std::string& name );

        /// return the offset ot apply to each wedgeindex so that
        /// after garbageCollection all indices are valid and coherent.
        std::vector<int> computeCleanupOffset() const;

        /// remove unreferenced wedge, halfedges need to be reindexed.
        inline void garbageCollection();

        /// \todo removeDuplicateWedge
        /// merge wedges with same data
        /// return old->new index correspondance to update wedgeIndexPph
        /// inline void removeDuplicateWedge
        inline size_t size() const { return m_data.size(); }

        /// attrib names associated to vertex/wedges, getted from CoreMesh, if any,
        std::vector<std::string> m_floatAttribNames;
        std::vector<std::string> m_vector2AttribNames;
        std::vector<std::string> m_vector3AttribNames;
        std::vector<std::string> m_vector4AttribNames;

        /// attrib handle from the CoreMesh given at construction, if any.
        std::vector<AttribHandle<float>> m_wedgeFloatAttribHandles;
        std::vector<AttribHandle<Vector2>> m_wedgeVector2AttribHandles;
        std::vector<AttribHandle<Vector3>> m_wedgeVector3AttribHandles;
        std::vector<AttribHandle<Vector4>> m_wedgeVector4AttribHandles;

        //      private:
        AlignedStdVector<Wedge> m_data;
    };

    WedgeData interpolateWedgeAttributes( const WedgeData&, const WedgeData&, Scalar alpha );

    template <typename T>
    inline void copyAttribToWedgeData( const TriangleMesh& mesh,
                                       unsigned int vindex,
                                       const std::vector<AttribHandle<T>>& attrHandleVec,
                                       VectorArray<T>* to );

    inline void copyMeshToWedgeData( const TriangleMesh& mesh,
                                     unsigned int vindex,
                                     const std::vector<AttribHandle<float>>& wprop_float,
                                     const std::vector<AttribHandle<Vector2>>& wprop_vec2,
                                     const std::vector<AttribHandle<Vector3>>& wprop_vec3,
                                     const std::vector<AttribHandle<Vector4>>& wprop_vec4,
                                     TopologicalMesh::WedgeData* wd );

    template <typename T>
    using HandleAndValueVector =
        std::vector<std::pair<AttribHandle<T>, T>,
                    Eigen::aligned_allocator<std::pair<AttribHandle<T>, T>>>;

    template <typename T>
    using PropPair = std::pair<AttribHandle<T>, OpenMesh::HPropHandleT<T>>;

    template <typename T>
    [[deprecated]] inline void copyAttribToTopo( const TriangleMesh& triMesh,
                                                 const std::vector<PropPair<T>>& vprop,
                                                 TopologicalMesh::HalfedgeHandle heh,
                                                 unsigned int vindex );

    template <typename T>
    [[deprecated]] inline void addAttribPairToTopo( const TriangleMesh& triMesh,
                                                    AttribManager::pointer_type attr,
                                                    std::vector<PropPair<T>>& vprop,
                                                    std::vector<OpenMesh::HPropHandleT<T>>& pph );

    void split_copy( EdgeHandle _eh, VertexHandle _vh );
    void split( EdgeHandle _eh, VertexHandle _vh );

    OpenMesh::HPropHandleT<WedgeIndex> m_wedgeIndexPph; /**< Halfedges' Wedge index */
    WedgeCollection m_wedges;                           /**< Wedge data management */

    ///\todo to be deleted/updated
    OpenMesh::HPropHandleT<Index> m_inputTriangleMeshIndexPph;
    OpenMesh::HPropHandleT<Index> m_outputTriangleMeshIndexPph;
    [[deprecated]] std::vector<OpenMesh::HPropHandleT<float>> m_floatPph;
    [[deprecated]] std::vector<OpenMesh::HPropHandleT<Vector2>> m_vec2Pph;
    [[deprecated]] std::vector<OpenMesh::HPropHandleT<Vector3>> m_vec3Pph;
    [[deprecated]] std::vector<OpenMesh::HPropHandleT<Vector4>> m_vec4Pph;

    friend class TMOperations;
};

// heplers
void printWedgesInfo( const TopologicalMesh& );

} // namespace Geometry
} // namespace Core
} // namespace Ra

#include <Core/Geometry/TopologicalMesh.inl>
