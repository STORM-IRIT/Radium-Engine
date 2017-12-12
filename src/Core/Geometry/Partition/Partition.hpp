#ifndef PARTITION_DEFINITION
#define PARTITION_DEFINITION

#include <set>

#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Mesh/MeshTypes.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Animation/Handle/HandleWeight.hpp>

namespace Ra {
    namespace Core {
        namespace Geometry {
            
            // Defining a Index ( useless ), representing the Index of a handle transform
            typedef uint Index;
            
            // Defining the VertexSegment, representing a set of vertices indices of the mesh, hence a segment of the mesh
            typedef std::vector< VertexIdx > VertexSegment;
            
            // Defining the TriangleSegment, representing a set of triangles indices of the mesh, hence a segment of the mesh
            typedef std::vector< TriangleIdx > TriangleSegment;
            
            // Defining a BitSet, where true indicates that the i-th vertex is part of the segment
            typedef std::vector< bool > BitSet;
            
            
            typedef std::vector< TriangleMesh > MeshPartition;
            
            
            
            /*
             * Return the VertexSegment from the given set of weight, for the given id.
             * If is_max is true, only the vertices where id is the most influent one will be returned.
             */
            VertexSegment extractVertexSegment( const Animation::MeshWeight& weights, const Index& id, const bool use_max = true );
            
            
            
            /*
             * Return the VertexSegment from the given set of weight, for the given id.
             * If is_max is true, only the vertices where id is the most influent one will be returned.
             */
            VertexSegment extractVertexSegment( const Animation::WeightMatrix& weights, const Index& id, const bool use_max = true );
            
            
            
            /*
             * Return the BitSet from the given VertexSegment, for a mesh with vertex_size vertices.
             */
            BitSet extractBitSet( const VertexSegment& v, const uint vertex_size );
            
            
            
            /*
             * Return the TriangleSegment from the given BitSet, for the given triangles.
             */
            TriangleSegment extractTriangleSegment( const BitSet& bit, const VectorArray< Triangle >& t );
            
            
            
            MeshPartition partition( const TriangleMesh& mesh, const Animation::WeightMatrix& weight, const bool use_max = true );
            
            
            
        } // namespace Geometry
    } // namespace Core
} // namespace Ra

#endif // PARTITION_DEFINITION
