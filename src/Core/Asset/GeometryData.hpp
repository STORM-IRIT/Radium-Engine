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
    [[deprecated( "Use getMultiIndexedGeometry().verticesWithLock() "
                  "instead." )]] inline Vector3Array&
    getVertices();

    /// Return the (const) list of vertices.
    [[deprecated(
        "Use getMultiIndexedGeometry().vertices() instead. " )]] inline const Vector3Array&
    getVertices() const;

    /// Set the mesh vertices.
    /// \note Use getAttrib<Vector3ui>( Geometry::MeshAttrib::VERTEX_POSITION ).setData( vertexList
    /// ) instead.
    template <typename Container>
    [[deprecated( "Use getAttrib<Vector3ui>( Geometry::MeshAttrib::VERTEX_POSITION ).setData( "
                  "vertexList ) instead. " )]] inline void
    setVertices( const Container& vertexList );

    /// Return the list of lines.
    /// \note For line meshes only.
    [[deprecated( "Use findIndexDataWithLock<Vector2ui>( \"in_edge\" ) "
                  "instead." )]] inline Vector2uArray&
    getEdges();

    /// Return the list of lines.
    /// \note For line meshes only.
    [[deprecated( "Use getIndexedData<Vector2ui>( \"in_edge\" ) "
                  "instead." )]] inline const Vector2uArray&
    getEdges() const;

    /// Set the list of lines.
    /// \note For line meshes only.
    /// \note Use findIndexDataWithLock<Vector2ui>( "in_edge" ), then modify value of the
    /// vectorArray and unlock it. instead.
    template <typename Container>
    [[deprecated( "Use setIndexedData( GeometryType::LINE_MESH, edgeList, \"in_edge\" ) "
                  "instead." )]] inline void
    setEdges( const Container& edgeList );

    /// Return the list of faces.
    /// \note For triangle/quadrangle/polygonal meshes only.
    [[deprecated( "Use findIndexDataWithLock<VectorNui>( \"in_face\" ) "
                  "instead." )]] inline VectorNuArray&
    getFaces();

    /// Return the list of faces.
    /// \note For triangle/quadrangle/polygonal meshes only.
    [[deprecated( "Use getIndexedData<VectorNui>( \"in_face\" ) "
                  "instead." )]] inline const VectorNuArray&
    getFaces() const;

    /// Set the list of faces.
    /// \note For triangle/quadrangle/polygonal meshes only.
    /// \note Use findIndexDataWithLock<VectorNui>( "in_face" ), then modify value of the
    /// vectorArray and unlock it. instead.
    template <typename Container>
    [[deprecated( "Use setIndexedData( GeometryType::POLY_MESH, faceList, \"in_face\" ) "
                  "instead." )]] inline void
    setFaces( const Container& faceList );

    /// Return the list of polyhedra.
    /// \note For tetrahedron/hexahedron meshes only.
    [[deprecated( "Use findIndexDataWithLock<VectorNui>( \"in_polyhedron\" ) "
                  "instead." )]] inline VectorNuArray&
    getPolyhedra();

    /// Return the list of polyhedra.
    /// \note For tetrahedron/hexahedron meshes only.
    [[deprecated( "Use getIndexedData<VectorNui>( \"in_polyhedron\" ) "
                  "instead." )]] inline const VectorNuArray&
    getPolyhedra() const;

    /// Set the list of polyhedra.
    /// \note For tetrahedron/hexahedron meshes only.
    /// \note Use findIndexDataWithLock<VectorNui>( "in_polyhedron" ), then modify value of the
    /// vectorArray and unlock it. instead.
    template <typename Container>
    [[deprecated( "Use setIndexedData( GeometryType::POLY_MESH, polyList, \"in_polyhedron\" ) "
                  "instead." )]] inline void
    setPolyhedra( const Container& polyList );

    /// Return the list of vertex normals.
    [[deprecated(
        "Use getMultiIndexedGeometry().normalsWithLock() instead." )]] inline Vector3Array&
    getNormals();

    /// Return the (const) list of vertex normals.
    [[deprecated( "Use getMultiIndexedGeometry().normals() instead. " )]] inline const Vector3Array&
    getNormals() const;

    /// Set the vertex normals.
    /// \note Use getAttrib<Vector3ui>( Geometry::MeshAttrib::VERTEX_NORMAL ).setData( normalList )
    /// instead.
    template <typename Container>
    [[deprecated( "Use getMultiIndexedGeometry().setNormals( normalList  ) instead." )]] inline void
    setNormals( const Container& normalList );

    /// Return the list of vertex tangent vectors.
    [[deprecated(
        "Use getAttrib<Vector3>( Geometry::MeshAttrib::VERTEX_TANGENT ).getDataWithLock() "
        "instead." )]] inline Vector3Array&
    getTangents();

    /// Return the (const) list of vertex tangents vectors.
    [[deprecated( "Use getAttrib<Vector3>( Geometry::MeshAttrib::VERTEX_TANGENT ).data() "
                  "instead " )]] inline const Vector3Array&
    getTangents() const;

    /// Set the vertex tangent vectors.
    /// \note Use getAttrib<Vector3ui>( Geometry::MeshAttrib::VERTEX_TANGENT ).setData( tangentList
    /// ) instead.
    template <typename Container>
    [[deprecated( "Use getAttrib<Vector3ui>( Geometry::MeshAttrib::VERTEX_TANGENT ).setData( "
                  "tangentList ) instead." )]] inline void
    setTangents( const Container& tangentList );

    /// Return the list of vertex bitangent vectors.
    [[deprecated(
        "Use getAttrib<Vector3>( Geometry::MeshAttrib::VERTEX_BITANGENT ).getDataWithLock() "
        "instead." )]] inline Vector3Array&
    getBiTangents();

    /// Return the (const) list of vertex bitangent vectors.
    [[deprecated( "Use getAttrib<Vector3>( Geometry::MeshAttrib::VERTEX_BITANGENT "
                  ").data() " )]] inline const Vector3Array&
    getBiTangents() const;

    /// Set the vertex bitangent vectors.
    /// \note Use getAttrib<Vector3ui>( Geometry::MeshAttrib::VERTEX_BITANGENT ).setData(
    /// bitangentList ) instead.
    template <typename Container>
    [[deprecated( "Use getAttrib<Vector3ui>( Geometry::MeshAttrib::VERTEX_BITANGENT ).setData( "
                  "bitangentList ) instead." )]] inline void
    setBitangents( const Container& bitangentList );

    /// Return the list of vertex texture coordinates.
    [[deprecated(
        "Use getAttrib<Vector3>( Geometry::MeshAttrib::VERTEX_TEXCOORD ).getDataWithLock() "
        "instead." )]] inline Vector3Array&
    getTexCoords();

    /// Return the (const) list of vertex texture coordinates.
    [[deprecated( "Use getAttrib<Vector3>( Geometry::MeshAttrib::VERTEX_TEXCOORD "
                  ").data() " )]] inline const Vector3Array&
    getTexCoords() const;

    /// Set the vertex texture coordinates.
    /// \note Use getAttrib<Vector3ui>( Geometry::MeshAttrib::VERTEX_TEXCOORD ).setData(
    /// texCoordList ) instead.
    template <typename Container>
    [[deprecated( "Use getAttrib<Vector3ui>( Geometry::MeshAttrib::VERTEX_TEXCOORD ).setData( "
                  "texCoordList ) instead." )]] inline void
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

    /// Access to the multiIndexedGeometry;
    inline Geometry::MultiIndexedGeometry& getMultiIndexedGeometry();

    /// Access to the (const) multiIndexedGeometry;
    inline const Geometry::MultiIndexedGeometry& getMultiIndexedGeometry() const;

    /// Access to the attrib manager
    inline Utils::AttribManager& getAttribManager();

    /// Access to the (const) attrib manager
    inline const Utils::AttribManager& getAttribManager() const;

    /**
     *
     * @tparam T
     * @param name
     * @return Attrib<T> from m_multiIndexedGeometry.
     */
    template <typename T>
    inline Utils::Attrib<T>& getAttrib( const Geometry::MeshAttrib& name );

    /**
     *
     * @tparam T
     * @param name
     * @return Attrib<T> from m_multiIndexedGeometry.
     */
    template <typename T>
    inline const Utils::Attrib<T>& getAttrib( const Geometry::MeshAttrib& name ) const;

    /**
     *
     * @tparam V
     * @param name
     * @param firstOccurrence
     * @return Initialize (if necessary) and get a VectorArray<V> contain in layer.
     * @note If firstOccurrence == true, it allow to function to be more efficient but the name
     * isn't use to find the layer.
     * @warning This function lock the layer, user has read-write access, when done, call
     * indexedDataUnlock ( const GeometryType& type, const std::string& name ).
     */
    template <typename V>
    inline VectorArray<V>& findIndexDataWithLock( const std::string& name     = "",
                                                  const bool& firstOccurrence = true );

    /**
     *
     * @tparam V
     * @param name
     * @param firstOccurrence
     * @note If firstOccurrence == true, it allow to function to be more efficient but the name
     * isn't use to find the layer.
     * @return VectorArray<V>& stored in the layer.
     */
    template <typename V>
    inline const VectorArray<V>& getIndexedData( const std::string& name     = "",
                                                 const bool& firstOccurrence = true ) const;

    /**
     *
     * @param type
     * @param name
     * @warning Unlock a layer only if it was previously locked.
     */
    inline void indexedDataUnlock( const GeometryType& type, const std::string& name = "" );

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
     * @tparam Container
     * @param name
     * @param attribDataList
     * @brief Copy data from attribDataList into the attrib obtain with name.
     *
     */
    template <typename V>
    inline void setAttribData( const Geometry::MeshAttrib& name,
                               const VectorArray<V>& attribDataList );

    /**
     *
     * @tparam V
     * @param indexedDataList
     * @param type
     * @param name
     * @brief Copy data from indexedDataList into the layer obtain with name and type.
     */
    template <typename V>
    inline void setIndexedData( const GeometryType& type,
                                const VectorArray<V>& indexedDataList,
                                const std::string& name = "" );

    /**
     *
     * @tparam V
     * @param geomBase
     * @return VectorArray<V> contains into the layerBase.
     */
    template <typename V>
    inline VectorArray<V>& getDataFromLayerBase( Geometry::GeometryIndexLayerBase& geomBase );

    /**
     *
     * @tparam V
     * @param geomBase
     * @return const VectorArray<V> contains into the layerBase.
     */
    template <typename V>
    inline const VectorArray<V>&
    getDataFromLayerBase( const Geometry::GeometryIndexLayerBase& geomBase ) const;

    /**
     *
     * @tparam L
     * @param firstOccurrence
     * @param name
     * @return GeometryIndexLayerBase& from the given name.
     * @note This will try to get the first occurrence of the layer ignoring the given name for
     * optimization. Please put firstOccurrence to false if you provide a name.
     * @warning You must unlock the layer after using this function by using indexedDataUnlock(
     * const GeometryType& type, const std::string& name ).
     */
    template <typename L>
    inline Geometry::GeometryIndexLayerBase& getLayerBaseWithLock( const bool& firstOccurrence,
                                                                   const std::string& name );

    /**
     *
     * @tparam L
     * @param firstOccurrence
     * @param name
     * @return const GeometryIndexLayerBase& from the given name.
     * @note This will try to get the first occurrence of the layer ignoring the given name for
     * optimization. Please put firstOccurrence to false if you provide a name.
     */
    template <typename L>
    inline const Geometry::GeometryIndexLayerBase& getLayerBase( const bool& firstOccurrence,
                                                                 const std::string& name ) const;

    /**
     *
     * @tparam V
     * @tparam L
     * @param firstOccurrence
     * @param name
     * @return VectorArray<V>& and lock the layer associated to it.
     */
    template <typename V, typename L>
    inline VectorArray<V>& getIndexedDataWithLock( const bool& firstOccurrence,
                                                   const std::string& name = "" );
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#include <Core/Asset/GeometryData.inl>
