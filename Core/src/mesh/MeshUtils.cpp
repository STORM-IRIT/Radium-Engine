#include <mesh/MeshUtils.hpp>

namespace Ra
{
    void MeshUtils::getAutoNormals(TriangleMesh& mesh, VectorArray < Vector3 >& normalsOut)
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


    bool find_duplicates(const TriangleMesh& mesh, std::vector <uint>& duplicatesMap)
    {
        bool hasDuplicates = false;
        duplicatesMap.clear();
        const int numVerts = mesh.m_vertices.size();
        duplicatesMap.resize(numVerts, -1);

        VectorArray<Vector3>::const_iterator vertPos;
        VectorArray<Vector3>::const_iterator duplicatePos;
        for (int i = 0; i < numVerts; ++i)
        {
            // We look if we have previously seen the same vertex before in the array.
            const Vector3& vertex = mesh.m_vertices[i];
            vertPos = mesh.m_vertices.cbegin() + i;
            duplicatePos = std::find(mesh.m_vertices.cbegin(), vertPos, vertex);

            // The number in duplicates_map will be 'i' if no duplicates are found
            // up to v_i, or the index of the first vertex equal to v_i.
            duplicatesMap[i] = (duplicatePos - mesh.m_vertices.cbegin());
            CORE_ASSERT(duplicatesMap[i] >= 0 && duplicatesMap[i] <= i, " Invalid vertex indices");
            hasDuplicates = (hasDuplicates || duplicatesMap[i] != i);
        }
        return hasDuplicates;
    }


}
