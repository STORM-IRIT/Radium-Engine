#ifndef RADIUM_CONVERT_TESTS_HPP_
#define RADIUM_CONVERT_TESTS_HPP_

#include <Tests/CoreTests/Tests.hpp>
#include <Core/Mesh/MeshPrimitives.hpp>
#include <Core/Mesh/Wrapper/TopologicalMeshConvert.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>

using Ra::Core::TriangleMesh;
using Ra::Core::MeshConverter;
using Ra::Core::TopologicalMesh;

namespace RaTests {
    
    class ConvertTests : public Test
    {
        void run() override
        {
            TriangleMesh newMesh;
            TriangleMesh mesh;
            TopologicalMesh topologicalMesh;
            
            //Test for close mesh
            mesh  = Ra::Core::MeshUtils::makeBox();
            Ra::Core::MeshConverter::convert(mesh, topologicalMesh);
            Ra::Core::MeshConverter::convert(topologicalMesh, newMesh);
            RA_UNIT_TEST( isSameMesh(mesh,newMesh), "Conversion to topological box mesh failed" );
            
            //Test for mesh with boundaries
            mesh  = Ra::Core::MeshUtils::makePlaneGrid(2,2);
            Ra::Core::MeshConverter::convert(mesh, topologicalMesh);
            Ra::Core::MeshConverter::convert(topologicalMesh, newMesh);
            RA_UNIT_TEST( isSameMesh(mesh,newMesh), "Conversion to topological grid mesh failed" );
            
            mesh  = Ra::Core::MeshUtils::makeCylinder(Vector3(0,0,0),Vector3(0,0,1), 1);
            Ra::Core::MeshConverter::convert(mesh, topologicalMesh);
            Ra::Core::MeshConverter::convert(topologicalMesh, newMesh);
            RA_UNIT_TEST( isSameMesh(mesh,newMesh), "Conversion to topological cylinder mesh failed" );
        }
        
        bool isSameMesh(TriangleMesh& meshOne,TriangleMesh& meshTwo)
        {
            bool result = true;
            int i = 0;
            //Check length
            if(meshOne.m_vertices.size() != meshTwo.m_vertices.size())
                return false;
            if(meshOne.m_normals.size() != meshTwo.m_normals.size())
                return false;
            if(meshOne.m_triangles.size() != meshTwo.m_triangles.size())
                return false;
            
            //Check triangles
            std::vector<Ra::Core::Vector3> stackVertices;
            
            i = 0;
            while(result && i < meshOne.m_triangles.size())
            {
                std::vector<Ra::Core::Vector3>::iterator it;
                stackVertices.clear();
                stackVertices.push_back(meshOne.m_vertices[meshOne.m_triangles[i][0]]);
                stackVertices.push_back(meshOne.m_vertices[meshOne.m_triangles[i][1]]);
                stackVertices.push_back(meshOne.m_vertices[meshOne.m_triangles[i][2]]);
                for(int j=0;j<3;++j)
                {
                    it = find(stackVertices.begin(), stackVertices.end(), meshTwo.m_vertices[meshTwo.m_triangles[i][j]]);
                    if (it != stackVertices.end())
                    {
                        stackVertices.erase(it);
                    }
                    else
                    {
                        result = false;
                    }
                }
                ++i;
            }
            return result;
        }
    };
    RA_TEST_CLASS(ConvertTests);
}


#endif // RADIUM_CONVERT_TESTS_HPP_
