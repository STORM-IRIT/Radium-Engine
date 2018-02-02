#ifndef RADIUMENGINE_GEOMETRY_DATA_HPP
#define RADIUMENGINE_GEOMETRY_DATA_HPP

#include <string>
#include <vector>
#include <memory>

#include <Core/RaCore.hpp>
#include <Core/Index/Index.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/VectorArray.hpp>

#include <Core/File/MaterialData.hpp>
#include <Core/File/AssetData.hpp>

namespace Ra {
    namespace Asset {

        class RA_CORE_API GeometryData : public AssetData
        {

        public:
            using Vector3Array  = Core::VectorArray<Core::Vector3>;
            using Vector2iArray = Core::VectorArray<Core::Vector2i>;
            using Vector2uArray = Core::VectorArray<Core::Vector2ui>;
            using VectorNiArray = Core::VectorArray<Core::VectorNi>;
            using VectorNuArray = Core::VectorArray<Core::VectorNui>;
            using Vector4Array  = Core::VectorArray<Core::Vector4>;
            using ColorArray    = Core::VectorArray<Core::Color>;

            using Weight        = std::pair<Scalar, uint>;
            using VertexWeights = std::vector<Weight>;
            using WeightArray   = std::vector<VertexWeights>;

            using DuplicateTable = std::vector<Ra::Core::Index>;

        public:

            RA_CORE_ALIGNED_NEW

            /// ENUM
            enum GeometryType
            {
                UNKNOWN = 1 << 0,
                POINT_CLOUD = 1 << 1,
                LINE_MESH = 1 << 2,
                TRI_MESH = 1 << 3,
                QUAD_MESH = 1 << 4,
                POLY_MESH = 1 << 5,
                TETRA_MESH = 1 << 6,
                HEX_MESH = 1 << 7
            };

            /// CONSTRUCTOR
            GeometryData(const std::string &name = "",
                         const GeometryType &type = UNKNOWN);

            GeometryData(const GeometryData &data) = default;

            /// DESTRUCTOR
            ~GeometryData();

            /// NAME
            inline void setName(const std::string &name);

            /// TYPE
            inline GeometryType getType() const;
            inline void setType(const GeometryType &type);

            /// FRAME
            inline Core::Transform getFrame() const;
            inline void setFrame(const Core::Transform &frame);

            /// DATA
            inline uint getVerticesSize() const;

            inline Vector3Array &getVertices();
            inline const Vector3Array &getVertices() const;
            // Copy data from vertexList. In-place setting with getVertices is preferred.
            template<typename Container>
            inline void setVertices(const Container &vertexList);

            inline Vector2uArray &getEdges();
            inline const Vector2uArray &getEdges() const;
            // Copy data from edgeList. In-place setting with getEdges is preferred.
            template<typename Container>
            inline void setEdges(const Container &edgeList);

            inline VectorNuArray &getFaces();
            inline const VectorNuArray &getFaces() const;
            // Copy data from edgeList. In-place setting with getEdges is preferred.
            template<typename Container>
            inline void setFaces(const Container &faceList);

            inline VectorNuArray &getPolyhedra();
            inline const VectorNuArray &getPolyhedra() const;
            // Copy data from polyList. In-place setting with getPolyhedra is preferred.
            template<typename Container>
            inline void setPolyhedron(const Container &polyList);

            inline Vector3Array &getNormals();
            inline const Vector3Array &getNormals() const;
            // Copy data from normalList. In-place setting with getNormals is preferred.
            template<typename Container>
            inline void setNormals(const Container &normalList);

            inline Vector3Array &getTangents();
            inline const Vector3Array &getTangents() const;
            // Copy data from tangentList. In-place setting with getTangents is preferred.
            template<typename Container>
            inline void setTangents(const Container &tangentList);

            inline Vector3Array &getBiTangents();
            inline const Vector3Array &getBiTangents() const;
            // Copy data from bitangentList. In-place setting with getBiTangents is preferred.
            template<typename Container>
            inline void setBitangents(const Container &bitangentList);

            inline Vector3Array &getTexCoords();
            inline const Vector3Array &getTexCoords() const;
            // Copy data from texCoordList. In-place setting with getTexCoords is preferred.
            template<typename Container>
            inline void setTextureCoordinates(const Container &texCoordList);

            inline ColorArray &getColors();
            inline const ColorArray &getColors() const;
            // Copy data from colorList. In-place setting with getColors is preferred.
            template<typename Container>
            inline void setColors(const Container &colorList);

            inline WeightArray &getWeights();
            inline const WeightArray &getWeights() const;
            // Copy data from weightList. In-place setting with getWeights is preferred.
            inline void setWeights(const WeightArray &weightList);

            inline const MaterialData &getMaterial() const;
            inline void setMaterial(MaterialData* material);

            /// DUPLICATES
            inline DuplicateTable &getDuplicateTable();
            inline const DuplicateTable &getDuplicateTable() const;
            inline void setDuplicateTable(const DuplicateTable &table);
            inline void setLoadDuplicates(const bool status);

            /// QUERY
            inline bool isPointCloud() const;
            inline bool isLineMesh() const;
            inline bool isTriMesh() const;
            inline bool isQuadMesh() const;
            inline bool isPolyMesh() const;
            inline bool isTetraMesh() const;
            inline bool isHexMesh() const;
            inline bool hasVertices() const;
            inline bool hasEdges() const;
            inline bool hasFaces() const;
            inline bool hasPolyhedra() const;
            inline bool hasNormals() const;
            inline bool hasTangents() const;
            inline bool hasBiTangents() const;
            inline bool hasTextureCoordinates() const;
            inline bool hasColors() const;
            inline bool hasWeights() const;
            inline bool hasMaterial() const;
            inline bool isLoadingDuplicates() const;

            /// DEBUG
            inline void displayInfo() const;

        protected:
            /// VARIABLE
            Core::Transform m_frame;
            GeometryType m_type;

            Vector3Array m_vertex;
            Vector2uArray m_edge;
            VectorNuArray m_faces;
            VectorNuArray m_polyhedron;
            Vector3Array m_normal;
            Vector3Array m_tangent;
            Vector3Array m_bitangent;
            Vector3Array m_texCoord;
            ColorArray m_color;
            WeightArray m_weights;

            std::shared_ptr<MaterialData> m_material;
            bool m_hasMaterial;

            // the duplicate table for vertices, according to all loaded File Data.
            // this table is used, when not loading duplicates, to fix vertices indices in edges and faces.
            // Note: if loading duplicates this table is a 1-1 correspondance, i.e. m_duplicateTable[i] == i .
            DuplicateTable m_duplicateTable;
            bool m_loadDuplicates;
        };

    } // namespace Asset
} // namespace Ra

#include <Core/File/GeometryData.inl>

#endif // RADIUMENGINE_GEOMETRY_DATA_HPP
