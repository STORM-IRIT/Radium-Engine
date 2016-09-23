#include <Core/Mesh/MeshUtils.hpp>

#include <utility>
#include <set>
#include <map>

#include <Core/Math/Math.hpp>
#include <Core/Math/RayCast.hpp>
#include <Core/String/StringUtils.hpp>

namespace Ra
{
    namespace Core
    {
        namespace MeshUtils
        {
            void getAutoNormals( TriangleMesh& mesh, VectorArray<Vector3>& normalsOut )
            {
                const uint numVertices = mesh.m_vertices.size();
                const uint numTriangles = mesh.m_triangles.size();

                normalsOut.clear();
                normalsOut.resize( numVertices, Vector3::Zero() );

                for ( uint t = 0; t < numTriangles; t++ )
                {
                    const Triangle& tri = mesh.m_triangles[t];
                    Vector3 n = getTriangleNormal( mesh, t );

                    for ( uint i = 0; i < 3; ++i )
                    {
                        normalsOut[tri[i]] += n;
                    }
                }

                normalsOut.getMap().colwise().normalize();
            }


            bool findDuplicates( const TriangleMesh& mesh, std::vector<VertexIdx>& duplicatesMap )
            {
                bool hasDuplicates = false;
                duplicatesMap.clear();
                const uint numVerts = mesh.m_vertices.size();
                duplicatesMap.resize( numVerts,VertexIdx(-1) );

                VectorArray<Vector3>::const_iterator vertPos;
                VectorArray<Vector3>::const_iterator duplicatePos;
                for ( uint i = 0; i < numVerts; ++i )
                {
                    // We look if we have previously seen the same vertex before in the array.
                    const Vector3& vertex = mesh.m_vertices[i];
                    vertPos = mesh.m_vertices.cbegin() + i;
                    duplicatePos = std::find( mesh.m_vertices.cbegin(), vertPos, vertex );

                    // The number in duplicates_map will be 'i' if no duplicates are found
                    // up to v_i, or the index of the first vertex equal to v_i.
                    duplicatesMap[i] = ( duplicatePos - mesh.m_vertices.cbegin() );
                    CORE_ASSERT( duplicatesMap[i] <= i, " Invalid vertex indices" );
                    hasDuplicates = ( hasDuplicates || duplicatesMap[i] != i );
                }
                return hasDuplicates;
            }

            void removeDuplicates(TriangleMesh& mesh, std::vector<VertexIdx>& vertexMap)
            {
                std::vector<VertexIdx> duplicatesMap;
                findDuplicates(mesh, duplicatesMap);

                std::vector<VertexIdx> newIndices(mesh.m_vertices.size(), VertexIdx(-1));
                Vector3Array uniqueVertices;
                for (uint i = 0; i < mesh.m_vertices.size(); i++)
                {
                    if (duplicatesMap[i] == i)
                    {
                        newIndices[i] = uniqueVertices.size();
                        uniqueVertices.push_back(mesh.m_vertices[i]);

                    }
                }

                for (uint i = 0; i < mesh.m_triangles.size(); i++)
                {
                    for (uint j = 0; j < 3; j++)
                    {
                        int oldIdx = mesh.m_triangles[i](j);
                        int newIdx = newIndices[duplicatesMap[oldIdx]];
                        mesh.m_triangles[i](j) = newIdx;
                    }
                }

                vertexMap.resize(mesh.m_vertices.size());
                for (uint i = 0; i < mesh.m_vertices.size(); i++)
                    vertexMap[i] = newIndices[duplicatesMap[i]];

                mesh.m_vertices = uniqueVertices;
            }

            RayCastResult castRay(const TriangleMesh &mesh, const Ray &ray)
            {
                RayCastResult result;
                result.m_hitTriangle = -1;
                Scalar minT = std::numeric_limits<Scalar>::max();

                std::vector<Scalar> tValues;
                std::array<Vector3,3> v;
                for ( uint i = 0; i < mesh.m_triangles.size(); ++i)
                {
                    tValues.clear();
                    getTriangleVertices(mesh, i, v);
                    if ( RayCast::vsTriangle(ray, v[0], v[1], v[2], tValues) && tValues[0] < minT )
                    {
                        minT = tValues[0];
                        result.m_hitTriangle = int(i);
                    }
                }

                if (result.m_hitTriangle >= 0)
                {
                    Scalar minDist = std::numeric_limits<Scalar>::max();
                    std::array<Vector3,3> v;
                    getTriangleVertices(mesh, result.m_hitTriangle, v);
                    for (uint i = 0; i < 3; ++i)
                    {
                        Scalar dSq = (v[i] - ray.pointAt(minT)).squaredNorm();
                        if (dSq < minDist)
                        {
                            result.m_nearestVertex = mesh.m_triangles[result.m_hitTriangle][i];
                        }
                    }
                    result.m_t = minT;

                }

                return result;
            }

            /// Return the mean edge length of the given triangle mesh
            Scalar getMeanEdgeLength( const TriangleMesh& mesh ) {
                typedef std::pair< uint, uint > Key;
                std::set< Key > list;
                const uint size = mesh.m_triangles.size();
                uint   edgeSize   = 0;
                Scalar edgeLength = 0.0;
                #pragma omp parallel for
                for( int t = 0; t < int(size); ++t ) {
                    for( uint v = 0; v < 3; ++v ) {
                        const uint i = mesh.m_triangles[t][v];
                        const uint j = mesh.m_triangles[t][( v + 1 ) % 3];
                        Key k( ( ( i < j ) ? i : j ), ( ( i < j ) ? j : i ) );
                        Scalar length = ( mesh.m_vertices[i] - mesh.m_vertices[j] ).norm();
                        #pragma omp critical
                        {
                            auto it = list.find( k );
                            if( it == list.end() ) {
                                list.insert( k );
                                ++edgeSize;
                                edgeLength += length;
                            }
                        }
                    }
                }
                if( edgeSize != 0 ) {
                    return ( edgeLength / Scalar( edgeSize ) );
                }
                return 0.0;
            }

        } // namespace MeshUtils
    } // namespace Core
} // namespace Ra
