#include <Core/Mesh/MeshUtils.hpp>

#include <Core/Math/Math.hpp>
#include <Core/String/StringUtils.hpp>

namespace Ra { namespace Core
{
    namespace MeshUtils
    {
        void getAutoNormals(TriangleMesh& mesh, VectorArray<Vector3>& normalsOut)
        {
            const uint numVertices = mesh.m_vertices.size();
            const uint numTriangles = mesh.m_triangles.size();

            normalsOut.clear();
            normalsOut.resize(numVertices, Vector3::Zero());

            for (uint t = 0; t < numTriangles; t++)
            {
                const Triangle& tri = mesh.m_triangles[t];
                Vector3 n = -getTriangleNormal(mesh, t);

                for (uint i = 0; i < 3; ++i)
                {
                    normalsOut[tri[i]] += n;
                }
            }

            normalsOut.getMap().colwise().normalize();
        }


        bool find_duplicates(const TriangleMesh& mesh, std::vector<VertexIdx>& duplicatesMap)
        {
            bool hasDuplicates = false;
            duplicatesMap.clear();
            const uint numVerts = mesh.m_vertices.size();
            duplicatesMap.resize(numVerts, -1);

            VectorArray<Vector3>::const_iterator vertPos;
            VectorArray<Vector3>::const_iterator duplicatePos;
            for (uint i = 0; i < numVerts; ++i)
            {
                // We look if we have previously seen the same vertex before in the array.
                const Vector3& vertex = mesh.m_vertices[i];
                vertPos = mesh.m_vertices.cbegin() + i;
                duplicatePos = std::find(mesh.m_vertices.cbegin(), vertPos, vertex);

                // The number in duplicates_map will be 'i' if no duplicates are found
                // up to v_i, or the index of the first vertex equal to v_i.
                duplicatesMap[i] = (duplicatePos - mesh.m_vertices.cbegin());
                CORE_ASSERT(duplicatesMap[i] <= i, " Invalid vertex indices");
                hasDuplicates = (hasDuplicates || duplicatesMap[i] != i);
            }
            return hasDuplicates;
        }

        TriangleMesh makeXNormalQuad(const Vector2& halfExts)
        {
            TriangleMesh result;

            result.m_vertices = {
                Vector3(0.0, halfExts[0],-halfExts[1]),
                Vector3(0.0,-halfExts[0],-halfExts[1]),
                Vector3(0.0,-halfExts[0], halfExts[1]),
                Vector3(0.0, halfExts[0], halfExts[1])
            };

            result.m_triangles = {
                Triangle(0, 1, 2), Triangle(0, 2, 3)
            };

            return result;
        }

        TriangleMesh makeYNormalQuad(const Vector2& halfExts)
        {
            TriangleMesh result;

            result.m_vertices = {
                Vector3( halfExts[0], 0.0,-halfExts[1]),
                Vector3(-halfExts[0], 0.0,-halfExts[1]),
                Vector3(-halfExts[0], 0.0, halfExts[1]),
                Vector3( halfExts[0], 0.0, halfExts[1])
            };

            result.m_triangles = {
                Triangle(0, 1, 2), Triangle(0, 2, 3)
            };

            return result;
        }

        TriangleMesh makeZNormalQuad(const Vector2& halfExts)
        {
            TriangleMesh result;

            result.m_vertices = {
                Vector3( halfExts[0],-halfExts[1], 0.0),
                Vector3(-halfExts[0],-halfExts[1], 0.0),
                Vector3(-halfExts[0], halfExts[1], 0.0),
                Vector3( halfExts[0], halfExts[1], 0.0)
            };

            result.m_triangles = {
                Triangle(0, 1, 2), Triangle(0, 2, 3)
            };

            return result;
        }

        TriangleMesh makeBox(const Vector3& halfExts)
        {
            Aabb aabb(-halfExts, halfExts);
            return makeBox(aabb);
        }

        TriangleMesh makeBox(const Aabb& aabb)
        {
            TriangleMesh result;
            result.m_vertices = {
                    aabb.corner(Aabb::BottomLeftFloor),
                    aabb.corner(Aabb::BottomRightFloor),
                    aabb.corner(Aabb::TopLeftFloor),
                    aabb.corner(Aabb::TopRightFloor),
                    aabb.corner(Aabb::BottomLeftCeil),
                    aabb.corner(Aabb::BottomRightCeil),
                    aabb.corner(Aabb::TopLeftCeil),
                    aabb.corner(Aabb::TopRightCeil)};
            result.m_triangles = {
                    Triangle(0, 2, 1), Triangle(1, 2, 3), // Floor
                    Triangle(0, 1, 4), Triangle(4, 1, 5), // Front
                    Triangle(3, 2, 6), Triangle(3, 6, 7), // Back
                    Triangle(5, 1, 3), Triangle(5, 3, 7), // Right
                    Triangle(2, 0, 4), Triangle(2, 4, 6), // Left
                    Triangle(4, 5, 6), Triangle(6, 5, 7)  // Top
            };

            getAutoNormals(result, result.m_normals);
            checkConsistency(result);
            return result;

        }

        TriangleMesh makeGeodesicSphere(Scalar radius, uint numSubdiv)
        {
            TriangleMesh result;
            // First, make an icosahedron.
            // Top vertex
            result.m_vertices.push_back(Vector3(0,0,radius));

            const Scalar sq5_5 = radius * std::sqrt(5.f)/5.f;

            // Middle vertices are on pentagons inscribed on a circle of radius 2*sqrt(5)
            for (int i = 0; i < 5 ; ++i)
            {
                for (int j = 0; j < 2 ; ++j)
                {
                    const Scalar theta = (Scalar(i) + (j*0.5f)) * Math::PiMul2 / 5.f;

                    const Scalar x = 2.f * sq5_5 * std::cos(theta);
                    const Scalar y = 2.f * sq5_5 * std::sin(theta);
                    const Scalar z = j ==0 ? sq5_5 : -sq5_5;
                    result.m_vertices.push_back(Vector3(x,y,z));
                }
            }

            // Bottom vertex
            result.m_vertices.push_back(Vector3(0,0,-radius));

            for (int i = 0; i < 5; ++i)
            {
                uint i1 = (i+1)%5;
                // Top triangles
                result.m_triangles.push_back(Triangle(0, 2*i+1, (2*i1 +1)));

                // Bottom triangles
                result.m_triangles.push_back(Triangle(2*i+2, 11, (2*i1 +2)));

            }
            for (uint i = 0; i < 10; ++i)
            {
                uint i1 = (i+0) %10 +1;
                uint i2 = (i+1) %10 +1;
                uint i3 = (i+2) %10 +1;
                i %2 ?  result.m_triangles.push_back(Triangle(i3,i2,i1))
                      : result.m_triangles.push_back(Triangle(i2,i3,i1));
            }


            for (uint n = 0; n < numSubdiv; ++n)
            {
                VectorArray<Triangle> newTris= result.m_triangles;
                // Now subdivide every face into 4 triangles.
                for (uint i = 0 ; i < result.m_triangles.size(); ++i)
                {
                    const Triangle& tri = result.m_triangles[i];
                    std::array<Vector3,3> triVertices;
                    std::array<uint,3> middles;

                    getTriangleVertices(result,i, triVertices);

                    for (int v = 0; v < 3 ; ++v)
                    {
                        middles[v] = result.m_vertices.size();
                        result.m_vertices.push_back(0.5f* (triVertices[v] + triVertices[(v+1)%3]));
                    }

                    newTris.push_back(Triangle(tri[0],middles[0], middles[2]));
                    newTris.push_back(Triangle(middles[0], tri[1], middles[1]));
                    newTris.push_back(Triangle(middles[2], middles[1], tri[2]));
                    newTris.push_back(Triangle(middles[0], middles[1], middles[2]));

                }
                result.m_triangles = newTris;
            }

            // Project vertices on the sphere
            for (auto& vertex : result.m_vertices)
            {
                const Scalar r = radius / vertex.norm();
                vertex *= r;
            }
            checkConsistency(result);
            return result;
        }


        void checkConsistency(const TriangleMesh& mesh)
        {
        #ifdef CORE_DEBUG
            std::vector<bool> visited(mesh.m_vertices.size(), false);
            for (uint t = 0 ; t < mesh.m_triangles.size(); ++t)
            {
                const Triangle& tri = mesh.m_triangles[t];
                for (uint i = 0; i < 3; ++i)
                {
                    std::string errStr;
                    StringUtils::stringPrintf(errStr, "Vertex %d is in triangle %d (#%d) is out of bounds", tri[i],t,i);
                    CORE_ASSERT(uint(tri[i]) < mesh.m_vertices.size(), errStr.c_str());
                    visited[tri[i]] = true;
                }
            }

            for (uint v = 0; v < visited.size(); ++v)
            {
                std::string errStr;
                StringUtils::stringPrintf(errStr, "Vertex %d does not belong to any triangle", v);
                CORE_ASSERT(visited[v], errStr.c_str());
            }

            // Normals are optional but if they are present then every vertex should have one.
            CORE_ASSERT(mesh.m_normals.size() ==  0 || mesh.m_normals.size() == mesh.m_vertices.size(),
            "Inconsistent number of normals");
        #endif
        }

    }
}}
