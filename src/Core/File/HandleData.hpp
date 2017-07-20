#ifndef RADIUMENGINE_HANDLE_DATA_HPP
#define RADIUMENGINE_HANDLE_DATA_HPP

#include <string>
#include <map>
#include <vector>

#include <Core/RaCore.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/AlignedStdVector.hpp>

#include <Core/File/AssetData.hpp>

namespace Ra {
    namespace Asset {

        struct RA_CORE_API HandleComponentData {
            RA_CORE_ALIGNED_NEW

            HandleComponentData();

            Core::Transform                          m_frame;
            std::string                              m_name;
            std::vector< std::pair< uint, Scalar > > m_weight;
        };

        class RA_CORE_API HandleData : public AssetData {
        public:
            RA_CORE_ALIGNED_NEW

            /// ENUM
            enum HandleType {
                UNKNOWN     = 1 << 0,
                POINT_CLOUD = 1 << 1,
                SKELETON    = 1 << 2,
                CAGE        = 1 << 3
            };

            /// CONSTRUCTOR
            HandleData( const std::string& name = "",
                        const HandleType&  type = UNKNOWN );

            HandleData( const HandleData& data ) = default;

            /// DESTRUCTOR
            ~HandleData();

            /// NAME
            inline void setName( const std::string& name );

            /// TYPE
            inline HandleType getType() const;
            inline void setType( const HandleType& type );

            /// FRAME
            inline Core::Transform getFrame() const;
            inline void setFrame( const Core::Transform& frame );

            /// VERTEX SIZE
            inline uint getVertexSize() const;
            inline void setVertexSize( uint size );

            /// NAME TABLE
            inline void setNameTable( const std::map< std::string, uint >& nameTable );

            /// DATA
            inline uint getComponentDataSize() const;
            inline const Core::AlignedStdVector<HandleComponentData>& getComponentData() const;
            inline       Core::AlignedStdVector<HandleComponentData>& getComponentData();
            inline const HandleComponentData& getComponent( const uint i ) const;
            inline       HandleComponentData& getComponent( const uint i );
            inline void setComponents( const Core::AlignedStdVector< HandleComponentData >& components );
            inline const Core::AlignedStdVector<Core::Vector2i>& getEdgeData() const;
            inline       Core::AlignedStdVector<Core::Vector2i>& getEdgeData();
            inline void setEdges( const Core::AlignedStdVector< Core::Vector2i >& edgeList );
            inline const Core::AlignedStdVector<Core::VectorNi>& getFaceData() const;
            inline       Core::AlignedStdVector<Core::VectorNi>& getFaceData();
            inline void setFaces( const Core::AlignedStdVector< Core::VectorNi >& faceList );
            inline void recomputeAllIndices();

            /// QUERY
            inline bool isPointCloud() const;
            inline bool isSkeleton() const;
            inline bool isCage() const;
            inline bool hasComponents() const;
            inline bool hasEdges() const;
            inline bool hasFaces() const;
            inline bool needsEndNodes() const;
            inline int  getIndexOf( const std::string& name ) const;

            inline void needEndNodes( bool need );

            /// DEBUG
            inline void displayInfo() const;

        protected:
            /// VARIABLE
            Core::Transform                    m_frame;
            HandleType                         m_type;

            bool                               m_endNode;
            uint                               m_vertexSize;
            std::map< std::string, uint >      m_nameTable;

            Core::AlignedStdVector< HandleComponentData > m_component;
            Core::AlignedStdVector< Core::Vector2i >      m_edge;
            Core::AlignedStdVector< Core::VectorNi >      m_face;
        };

    } // namespace Asset
} // namespace Ra

#include <Core/File/HandleData.inl>

#endif // RADIUMENGINE_HANDLE_DATA_HPP
