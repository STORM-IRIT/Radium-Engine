#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Core/Containers/VectorArray.hpp>
#include <Core/RaCore.hpp>
#include <Core/Types.hpp>
#include <Core/Utils/Attribs.hpp>
#include <Core/Utils/Index.hpp>

#include <Core/Asset/AssetData.hpp>
#include <Core/Asset/MaterialData.hpp>
#include <Core/Geometry/TriangleMesh.hpp>

namespace Ra {
namespace Core {
namespace Asset {

class MaterialData;

/**
 * The GeometryData class stores all the geometry related data of a loaded object.
 */
class RA_CORE_API GeometryData : public AssetData
{
  public:
    using ColorArray = Vector4Array;

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /**
     * The type of geometry.
     */
    enum GeometryType {
        UNKNOWN     = 1 << 0,
        POINT_CLOUD = 1 << 1,
        LINE_MESH   = 1 << 2,
        TRI_MESH    = 1 << 3,
        QUAD_MESH   = 1 << 4,
        POLY_MESH   = 1 << 5,
        TETRA_MESH  = 1 << 6,
        HEX_MESH    = 1 << 7
    };

    GeometryData( const std::string& name = "", const GeometryType& type = UNKNOWN );

    GeometryData( const GeometryData& data ) = delete;

    ~GeometryData();

    /// \name Data access
    /// \{

    /// Return the name of the object.
    inline void setName( const std::string& name );

    /// Return the type of geometry.
    inline GeometryType getType() const;

    /// Set the type of geometry.
    inline void setType( const GeometryType& type );

    /// Return the Transform of the object.
    inline Transform getFrame() const;

    /// Set the Transform of the object.
    inline void setFrame( const Transform& frame );

    /// Return the number of vertices.
    inline std::size_t getVerticesSize() const;

    /// Return the list of vertices.
    [[deprecated( "Use addAttribDataWithLock( std::string name ) instead." )]] inline Vector3Array&
    getVertices();

    /// Return the list of vertices.
    [[deprecated( "Use getAttribData( std::string name ) instead." )]] inline const Vector3Array&
    getVertices() const;

    /// Set the mesh vertices.
    /// \note In-place setting with getVertices() is preferred.
    template <typename Container>
    [[deprecated( "Use setAttribData( const Geometry::MeshAttrib& name, const VectorArray<V>& "
                  "attribDataList ) "
                  "instead." )]] inline void
    setVertices( const Container& vertexList );

    /// Return the list of lines.
    /// \note For line meshes only.
    [[deprecated( "Use addIndexedDataWithLock( const GeometryType& type, const std::string& name ) "
                  "instead." )]] inline Vector2uArray&
    getEdges();

    /// Return the list of lines.
    /// \note For line meshes only.
    [[deprecated( "Use getIndexedData( const GeometryType& type, const std::string& name ) "
                  "instead." )]] inline const Vector2uArray&
    getEdges() const;

    /// Set the list of lines.
    /// \note For line meshes only.
    /// \note In-place setting with getEdges() is preferred.
    template <typename Container>
    [[deprecated( "Use setIndexedData( const GeometryType& type, const VectorArray<V>& "
                  "indexedDataList, const std::string& name) instead. " )]] inline void
    setEdges( const Container& edgeList );

    /// Return the list of faces.
    /// \note For triangle/quadrangle/polygonal meshes only.
    [[deprecated( "Use addIndexedDataWithLock( const GeometryType& type, const std::string& name ) "
                  "instead." )]] inline VectorNuArray&
    getFaces();

    /// Return the list of faces.
    /// \note For triangle/quadrangle/polygonal meshes only.
    [[deprecated( "Use getIndexedData( const GeometryType& type, const std::string& name ) "
                  "instead." )]] inline const VectorNuArray&
    getFaces() const;

    /// Set the list of faces.
    /// \note For triangle/quadrangle/polygonal meshes only.
    /// \note In-place setting with getFaces() is preferred.
    template <typename Container>
    [[deprecated( "Use setIndexedData( const GeometryType& type, const VectorArray<V>& "
                  "indexedDataList, const std::string& name) instead. " )]] inline void
    setFaces( const Container& faceList );

    /// Return the list of polyhedra.
    /// \note For tetrahedron/hexahedron meshes only.
    [[deprecated( "Use addIndexedDataWithLock( const GeometryType& type, const std::string& name ) "
                  "instead." )]] inline VectorNuArray&
    getPolyhedra();

    /// Return the list of polyhedra.
    /// \note For tetrahedron/hexahedron meshes only.
    [[deprecated( "Use getIndexedData( const GeometryType& type, const std::string& name ) "
                  "instead." )]] inline const VectorNuArray&
    getPolyhedra() const;

    /// Set the list of polyhedra.
    /// \note For tetrahedron/hexahedron meshes only.
    /// \note In-place setting with getPolyhedra() is preferred.
    template <typename Container>
    [[deprecated( "Use setIndexedData( const GeometryType& type, const VectorArray<V>& "
                  "indexedDataList, const std::string& name) instead. " )]] inline void
    setPolyhedra( const Container& polyList );

    /// Return the list of vertex normals.
    [[deprecated( "Use addAttribDataWithLock( std::string name ) instead." )]] inline Vector3Array&
    getNormals();

    /// Set the vertex normals.
    /// \note In-place setting with getNormals() is preferred.
    template <typename Container>
    [[deprecated( "Use setAttribData( const Geometry::MeshAttrib& name, const VectorArray<V>& "
                  "attribDataList ) "
                  "instead." )]] inline void
    setNormals( const Container& normalList );

    /// Return the list of vertex tangent vectors.
    [[deprecated( "Use addAttribDataWithLock( std::string name ) instead." )]] inline Vector3Array&
    getTangents();

    /// Set the vertex tangent vectors.
    /// \note In-place setting with getTangents() is preferred.
    template <typename Container>
    [[deprecated( "Use setAttribData( const Geometry::MeshAttrib& name, const VectorArray<V>& "
                  "attribDataList ) "
                  "instead." )]] inline void
    setTangents( const Container& tangentList );

    /// Return the list of vertex bitangent vectors.
    [[deprecated( "Use addAttribDataWithLock( std::string name ) instead." )]] inline Vector3Array&
    getBiTangents();

    /// Set the vertex bitangent vectors.
    /// \note In-place setting with getBiTangents() is preferred.
    template <typename Container>
    [[deprecated( "Use setAttribData( const Geometry::MeshAttrib& name, const VectorArray<V>& "
                  "attribDataList ) "
                  "instead." )]] inline void
    setBitangents( const Container& bitangentList );

    /// Return the list of vertex texture coordinates.
    [[deprecated( "Use addAttribDataWithLock( std::string name ) instead." )]] inline Vector3Array&
    getTexCoords();

    /// Set the vertex texture coordinates.
    /// \note In-place setting with getTexCoords() is preferred.
    template <typename Container>
    [[deprecated( "Use setAttribData( const Geometry::MeshAttrib& name, const VectorArray<V>& "
                  "attribDataList ) "
                  "instead." )]] inline void
    setTextureCoordinates( const Container& texCoordList );

    /// Return the MaterialData associated to the objet.
    inline const MaterialData& getMaterial() const;

    /// Set the MaterialData for the object.
    inline void setMaterial( MaterialData* material );
    /// \}

    /// \name Status queries
    /// \{

    /// Return true if the object is a Point Cloud.
    inline bool isPointCloud() const;

    /// Return true if the object is a Line Mesh.
    inline bool isLineMesh() const;

    /// Return true if the object is a Triangle Mesh.
    inline bool isTriMesh() const;

    /// Return true if the object is a Quadrangle Mesh.
    inline bool isQuadMesh() const;

    /// Return true if the object is a Polygon Mesh.
    /// \note Return false for Triangle and Quadrangle meshes.
    inline bool isPolyMesh() const;

    /// Return true if the object is a Tetrahedron Mesh.
    inline bool isTetraMesh() const;

    /// Return true if the object is a Hexahedron Mesh.
    inline bool isHexMesh() const;

    /// Return true if the object has vertices.
    [[deprecated( "Use hasAttribData( std::string name ) instead." )]] inline bool
    hasVertices() const;

    /// Return true if the object has lines.
    inline bool hasEdges() const;

    /// Return true if the object has faces.
    inline bool hasFaces() const;

    /// Return true if the object has polyhedra.
    inline bool hasPolyhedra() const;

    /// Return true if the object has vertex normals.
    [[deprecated( "Use hasAttribData( std::string name ) instead." )]] inline bool
    hasNormals() const;

    /// Return true if the object has vertex tangent vectors.
    [[deprecated( "Use hasAttribData( std::string name ) instead." )]] inline bool
    hasTangents() const;

    /// Return true if the object has vertex bitangent vectors.
    [[deprecated( "Use hasAttribData( std::string name ) instead." )]] inline bool
    hasBiTangents() const;

    /// Return true if the object has vertex texture coordinates.
    [[deprecated( "Use hasAttribData( std::string name ) instead." )]] inline bool
    hasTextureCoordinates() const;

    /// Return true if the object has MaterialData.
    inline bool hasMaterial() const;
    /// \}

    /// Print stast info to the Debug output.
    void displayInfo() const;

    /// Access to the attrib manager
    inline Utils::AttribManager& getAttribManager();

    /// Access to the (const) attrib manager
    inline const Utils::AttribManager& getAttribManager() const;

    /**
     *
     * @tparam V
     * @param type
     * @param name
     * @return Initialize (if necessary) and get a VectorArray<V> contain in layer.
     * @note This function is only to avoid redundant code of function like getEdges().
     * @warning This function lock the layer, user has read-write access, when done, call
     * indexedDataUnlock ( const GeometryType& type, const std::string& name ).
     */
    template <typename V>
    inline VectorArray<V>& addIndexedDataWithLock( const GeometryType& type,
                                                   const std::string& name = "" );

    /**
     *
     * @tparam V
     * @param type
     * @param name
     * @note This function is only to avoid redundant code of function like getEdges().
     * @return VectorArray<V>& stored in the layer.
     */
    template <typename V>
    inline const VectorArray<V>& getIndexedData( const GeometryType& type,
                                                 const std::string& name = "" ) const;

    /**
     *
     * @param type
     * @param name
     * @warning Unlock a layer only if it was previously locked.
     */
    inline void indexedDataUnlock( const GeometryType& type, const std::string& name = "" );

    /**
     *
     * @tparam V
     * @param indexedDataList
     * @param type
     * @param name
     * @brief Copy data from indexedDataList into the layer obtain with name and type.
     * @note In-place setting with addIndexedDataWithLock( std::string name ) is preferred.
     * This function is only to avoid redundant code of function like setEdges().
     */
    template <typename V>
    inline void setIndexedData( const GeometryType& type,
                                const VectorArray<V>& indexedDataList,
                                const std::string& name = "" );

  protected:
    /// The transformation of the object.
    Transform m_frame;

    /// The type of geometry for the object.
    GeometryType m_type;

    /// Named attributes
    Core::Geometry::MultiIndexedGeometry m_multiIndexedGeometry;

    /// The MaterialData for the object.
    std::shared_ptr<MaterialData> m_material;

  private:
    /**
     *
     * @tparam V
     * @param name
     * @return Get container base on name (lock).
     * @warning If AttribHandle corresponding to name doesn't exist, it's created
     * and return. By using this method, user has read-write access to data, data is lock, when
     * done, user must unlock data by getting the attribManager and then get the attrib.
     * @note This function is only to avoid redundant code of function like getNormals().
     */
    template <typename V>
    inline VectorArray<V>& addAttribDataWithLock( const Geometry::MeshAttrib& name );

    /**
     *
     * @tparam Container
     * @param name
     * @param attribDataList
     * @brief Copy data from attribDataList into the attrib obtain with name.
     * @note In-place setting with addAttribDataWithLock( std::string name ) is preferred.
     * This function is only to avoid redundant code of function like setNormals().
     *
     */
    template <typename V>
    inline void setAttribData( const Geometry::MeshAttrib& name,
                               const VectorArray<V>& attribDataList );

    /**
     * @tparam V
     * @param name
     * @return true if the name provided correspond to an existing attribHandle.
     * @note This function is only to avoid redundant code of function like hasNormals().
     *
     */
    template <typename V>
    inline bool hasAttribData( const Geometry::MeshAttrib& name ) const;

    /**
     *
     * @tparam L
     * @param name
     * @return true if layer isn't initialized.
     */
    template <typename L>
    inline bool initLayer( const std::string& name );

    /**
     *
     * @tparam V
     * @tparam L
     * @param name
     * @return VectorArray<V>& and lock the layer associated to it.
     */
    template <typename V, typename L>
    inline VectorArray<V>& getIndexedDataWithLock( const std::string& name );

    /**
     *
     * @tparam V
     * @tparam L
     * @param name
     * @return VectorArray<V>&
     */
    template <typename V, typename L>
    inline const VectorArray<V>& getIndexedData( const std::string& name ) const;
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#include <Core/Asset/GeometryData.inl>
