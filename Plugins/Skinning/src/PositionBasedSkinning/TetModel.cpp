#include "TetModel.h"

#include <fstream>


using namespace PBD;
using namespace std;
using namespace Ra::Core;



TetModel::TetModel() :
    m_scaleFactorVolumetricMesh{1.1}
{
}

TetModel::~TetModel(void)
{
	cleanupModel();
}

void TetModel::cleanupModel()
{
    m_tetIndices.clear();
    m_edges.clear();
}

/* ------------------
  Load Tet Model
 -------------------- */

bool TetModel::openFile (const std::string &filename, ifstream &input)
{
    bool fileFound{true};

    input.open(filename.c_str());
    if(!input)
    {
        fileFound = false;
    }
    return fileFound;
}

void TetModel::extract_vertices (ifstream &input, ParticleData &pd)
{
    string line;
    stringstream sStream;
    unsigned nodeInd;
    Scalar x, y, z;

    // Vertices
    getline(input, line);

    // Number of vertices
    getline(input, line);
    sStream << line;
    sStream >> m_numPoints;
    std::cout << "Number of vertices : " << m_numPoints << std::endl;
    sStream.clear();

    pd.release();
    pd.reserve(m_numPoints);

    // read vertices
    for(auto i = 0; i < m_numPoints; ++i)
    {
        getline(input, line);
        sStream << line;
        sStream >> x >> y >> z >> nodeInd;
        sStream.clear();

        Vector3 vertex{Vector3(x, y, z) * m_scaleFactorVolumetricMesh};
        pd.addVertex(vertex);
    }
}

void TetModel::extract_triangles (ifstream &input)
{
    size_t num_triangles;
    string line;
    stringstream sStream;

    // Triangles
    getline(input, line);

    // Number of Triangles
    getline(input, line);
    sStream << line;
    sStream >> num_triangles;
    std::cout << "Number of Triangles : " << num_triangles << std::endl;
    sStream.clear();

    //read Triangles
    for(auto i = 0; i < num_triangles; ++i)
    {
        getline(input, line);
    }
}

void TetModel::extract_tetrahedrons (ifstream &input)
{
    size_t num_tetras;
    uint tetIndex1, tetIndex2, tetIndex3, tetIndex4, eleInd;
    string line;
    stringstream sStream;

    // Tetrahedra
    getline(input, line);
    // Number of Tetrahedrons
    getline(input, line);
    sStream << line;
    sStream >> num_tetras;
    std::cout << "Number of Tetrahedrons : " << num_tetras << std::endl;
    sStream.clear();

    m_tetIndices.resize(4u * num_tetras);
    m_edges.reserve(6u * num_tetras);
    m_tetIndices.clear();

    // read tetrahedra
    for(auto i = 0; i < num_tetras; ++i)
    {
        getline(input, line);
        sStream << line	;
        sStream >> tetIndex1 >> tetIndex2 >> tetIndex3 >> tetIndex4 >> eleInd;

        m_tetIndices.push_back(tetIndex1 - 1);
        m_tetIndices.push_back(tetIndex2 - 1);
        m_tetIndices.push_back(tetIndex3 - 1);
        m_tetIndices.push_back(tetIndex4 - 1);
        sStream.clear();
    }
}

bool TetModel::loadMeshModel(const std::string &filename, ParticleData &pd)
{
    std::cout << "Loading " << filename << std::endl;
    string line;
    ifstream input;

    bool fileFound = openFile(filename, input);

    if (fileFound)
    {
        //MeshVersionFormatted 1
        getline(input, line);
        // Dimension 3
        getline(input, line);

        extract_vertices(input, pd);
        extract_triangles(input);
        extract_tetrahedrons(input);

        // close file
        input.close();
    }

    return fileFound;
}

//--------------------------------------------------------------------------------------------------------

bool TetModel::initMesh(const std::string &filename, ParticleData &pd)
{
    bool fileFound = loadMeshModel(filename, pd);
    if (fileFound)
        buildNeighbors();
    return fileFound;
}

void TetModel::buildNeighbors()
{
    typedef std::vector<unsigned int> VertexEdges;
    VertexEdges* vEdges = new VertexEdges[numVertices()];    
    m_edges.clear();

    for(unsigned int i=0; i < numTets(); i++)
    {
        // tet edge indices: {0,1, 0,2, 0,3, 1,2, 1,3, 2,3}
        const unsigned int edges[12] = {	m_tetIndices[4*i], m_tetIndices[4*i+1],
                                            m_tetIndices[4*i], m_tetIndices[4*i+2],
                                            m_tetIndices[4*i], m_tetIndices[4*i+3],
                                            m_tetIndices[4*i+1], m_tetIndices[4*i+2],
                                            m_tetIndices[4*i+1], m_tetIndices[4*i+3],
                                            m_tetIndices[4*i+2], m_tetIndices[4*i+3]};

        for(unsigned int j=0u; j < 6; j++)
        {
            // add face information
            const unsigned int a = edges[j*2+0];
            const unsigned int b = edges[j*2+1];
            unsigned int edge = 0xffffffff;
            // find edge
            for(unsigned int k=0; k < vEdges[a].size(); k++)
            {
                // Check if we already have this edge in the list
                const Edge& e = m_edges[vEdges[a][k]];
                if(((e.m_vert[0] == a) || (e.m_vert[0] == b)) &&
                    ((e.m_vert[1] == a) || (e.m_vert[1] == b)))
                {
                    edge = vEdges[a][k];
                    break;
                }
            }

            if(edge == 0xffffffff)
            {
                // create new
                Edge e;
                e.m_vert[0] = a;
                e.m_vert[1] = b;
                m_edges.push_back(e);
                edge = (unsigned int) m_edges.size() - 1u;

                // add vertex-edge connection
                vEdges[a].push_back(edge);
                vEdges[b].push_back(edge);
            }
        }
    }
    delete [] vEdges;
}
