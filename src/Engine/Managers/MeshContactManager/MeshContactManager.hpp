#ifndef RADIUMENGINE_MESHCONTACTMANAGER_HPP
#define RADIUMENGINE_MESHCONTACTMANAGER_HPP

#include <Engine/Managers/MeshContactManager/MeshContactElement.hpp>

#include <Engine/System/System.hpp>

#include <Core/TreeStructures/kdtree.hpp>
#include <Core/TreeStructures/trianglekdtree.hpp>

#include <Core/Mesh/ProgressiveMesh/PriorityQueue.hpp>

#include <Core/Mesh/Wrapper/Convert.hpp>

#include <Core/Mesh/ProgressiveMesh/ProgressiveMeshData.hpp>

#include <Core/Mesh/ProgressiveMesh/ProgressiveMesh.hpp>

#include <Core/Geometry/Distance/DistanceQueries.hpp>

#define NBMAX_ELEMENTS 10

#define NBMAX_STEP 256


namespace Ra
{
    namespace Engine
    {

        class MeshContactManager
        {
        public:

            MeshContactManager();
            void setNbFacesChanged(const int nb);
            void setNbObjectsChanged(const int nb);
            void setThresholdChanged(const double threshold);
            void setLambdaChanged(const double lambda);
            void setMChanged(const double m);
            void setNChanged(const double n);
            void setLodValueChanged(int value);
            void setConstructM0();

            int getNbFacesMax();
            void computeNbFacesMax();
            void computeNbFacesMax2();

            void addMesh(MeshContactElement* mesh);

//            void computeThreshold();
//            void computeThresholdTest();
            void computeThresholdDistribution();
            void compareThresholdDistribution();

            void constructPriorityQueues();
            void updatePriorityQueue(Ra::Core::Index vsIndex, Ra::Core::Index vtIndex, int objIndex);
            void constructPriorityQueues2();
            void updatePriorityQueue2(Ra::Core::Index vsIndex, Ra::Core::Index vtIndex, int objIndex);
            bool edgeCollapse(int objIndex);

        private:

            int m_nb_faces_max;
            int m_nbfacesinit;
            int m_nbfaces;
            int m_nbobjects; // number of objects to be simplify (the first ones to be loaded into the scene)
            Scalar m_threshold; // distance used to define "contacts"
            //Scalar m_broader_threshold; // amplified threshold
            Scalar m_lambda; // influence of the original quadric considering contacts
            Scalar m_m;
            Scalar m_n; // slope of the weight function for contacts

            Eigen::Matrix<Scalar, NBMAX_ELEMENTS, NBMAX_ELEMENTS> m_thresholds; // thresholds for each pair of objects

            std::vector<Super4PCS::KdTree<>*> m_kdtrees;
            std::vector<Super4PCS::TriangleKdTree<>*> m_trianglekdtrees;
            std::vector<MeshContactElement*> m_meshContactElements;

            std::vector<Ra::Core::TriangleMesh> m_initTriangleMeshes; // used for contact computation

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
                        if (a1.m_fl_id != a2.m_fl_id)
                            return a1.m_fl_id < a2.m_fl_id;
                        else
                            return a1.m_vs_id + a1.m_vt_id < a2.m_vs_id + a2.m_vt_id;
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
