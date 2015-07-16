#include <Core/Mesh/MeshUtils.hpp>

#include <set>

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
                CORE_ASSERT( duplicatesMap[i] <= i, " Invalid vertex indices");
                hasDuplicates = (hasDuplicates || duplicatesMap[i] != i);
            }
            return hasDuplicates;
        }

        TriangleMesh makeBox(const Vector3& halfExts)
        {
            Aabb aabb (-halfExts, halfExts);
            return makeBox(aabb);
        }

        TriangleMesh makeBox(const Aabb &aabb)
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
                Triangle(0,2,1), Triangle(2,3,1), // Floor
                Triangle(0,1,4), Triangle(4,1,5), // Front
                Triangle(3,2,6), Triangle(3,6,7), // Back
                Triangle(1,5,3), Triangle(3,5,7), // Right
                Triangle(0,4,2), Triangle(2,4,6), // Left
                Triangle(4,5,6), Triangle(5,6,7)  // Top
            };

            getAutoNormals(result, result.m_normals);
            return result;

        }


    }
}}
