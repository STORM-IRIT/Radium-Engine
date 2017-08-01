#ifndef RADIUMENGINE_MESHCONTACTMANAGER_HPP
#define RADIUMENGINE_MESHCONTACTMANAGER_HPP

#include <Engine/Managers/MeshContactManager/MeshContactElement.hpp>

#include <Engine/System/System.hpp>

#include <Core/TreeStructures/kdtree.hpp>

#include <Core/Mesh/ProgressiveMesh/PriorityQueue.hpp>

//#include <Core/Mesh/Wrapper/Convert.hpp>
#include <Core/Mesh/Wrapper/TopologicalMeshConvert.hpp>

#include <Core/Mesh/ProgressiveMesh/ProgressiveMeshData.hpp>

#include <Core/Mesh/ProgressiveMesh/ProgressiveMesh.hpp>


namespace Ra
{
    namespace Engine
    {

        class MeshContactManager
        {
        public:

            MeshContactManager();
            void setNbFacesChanged(const int nb);
            void setThresholdChanged(const double threshold);
            void setLambdaChanged(const double lambda);
            void setLodValueChanged(int value);
            void setConstructM0();

            int getNbFacesMax();
            void computeNbFacesMax();

            void addMesh(MeshContactElement* mesh);

            void constructPriorityQueues();
            void updatePriorityQueue(Ra::Core::TopologicalMesh::VertexHandle vsHandle, Ra::Core::TopologicalMesh::VertexHandle vtHandle, int objIndex);
            bool edgeCollapse(int objIndex);

        private:

            int m_nb_faces_max;
            int m_nbfaces;
            Scalar m_threshold; // distance used to define "contacts"
            Scalar m_lambda; // influence of the original quadric considering contacts

            std::vector<Super4PCS::KdTree<Scalar>*> m_kdtrees;
            std::vector<MeshContactElement*> m_meshContactElements;


            struct comparePriorityQueueDataByErrorContact
            {
                typedef Ra::Core::PriorityQueue::PriorityQueueData PriorityQueueData;
                inline bool operator()(const PriorityQueueData &a1, const PriorityQueueData &a2) const
                {
                    if (a1.m_err != a2.m_err)
                        return a1.m_err < a2.m_err;
                    else
                    {
                        if (a1.m_index != a2.m_index)
                               return (a1.m_index < a2.m_index);
                        if (a1.m_fl.idx() != a2.m_fl.idx())
                            return a1.m_fl.idx() < a2.m_fl.idx();
                        else
                            return a1.m_vs.idx() + a1.m_vt.idx() < a2.m_vs.idx() + a2.m_vt.idx();
                    }
                }
            };

            typedef std::set<Ra::Core::PriorityQueue::PriorityQueueData, comparePriorityQueueDataByErrorContact> QueueContact;
            QueueContact m_mainqueue;

            std::vector<int> m_index_pmdata; // array recording in which objects collapses have been done
            int m_curr_vsplit; // index for m_index_pmdata
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_MESHCONTACTMANAGER_HPP
