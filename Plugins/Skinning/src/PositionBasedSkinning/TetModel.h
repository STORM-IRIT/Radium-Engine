#ifndef __TETMODEL_H__
#define __TETMODEL_H__

#include "Config.h"
#include "ParticleData.h"

namespace PBD
{
	class TetModel
	{
		public:
            struct Edge
            {
                unsigned int m_vert[2];
            };

            typedef std::vector<unsigned int> Tets;
            typedef std::vector<Edge> Edges;

			TetModel();
			virtual ~TetModel();

        private:
            const Scalar m_scaleFactorVolumetricMesh;
            unsigned int m_numPoints;
            Tets m_tetIndices;
            Edges m_edges;

            void cleanupModel();
		public:
            /* Load Tet Model */
            bool openFile(const std::string &filename, std::ifstream &input);
            void extract_vertices (std::ifstream &input, ParticleData &pd);
            void extract_triangles (std::ifstream &input);
            void extract_tetrahedrons (std::ifstream &input);
            bool loadMeshModel(const std::string &filename, ParticleData &pd);

            bool initMesh(const std::string &filename, ParticleData &pd);

            const Tets& getTets() const {return m_tetIndices;}
            Tets& getTets(){return m_tetIndices;}
            Edges& getEdges() {return m_edges;}
            const Edges& getEdges() const {return m_edges;}

            unsigned int numVertices() const { return m_numPoints; }
            unsigned int numTets() const { return (unsigned int)m_tetIndices.size() / 4; }

            void buildNeighbors();
	};
}

#endif
