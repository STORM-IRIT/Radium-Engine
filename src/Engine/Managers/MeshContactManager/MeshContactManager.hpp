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

#define PRECISION 1000


namespace Ra
{
    namespace Engine
    {

        class MeshContactManager
        {
            struct PtDistrib
            {
                Scalar r;
                Scalar a;
                Ra::Core::Index faceId;
                Ra::Core::Index objId;
            };

        public:

            MeshContactManager();
            void setNbFacesChanged(const int nb);
            void setNbObjectsChanged(const int nb);
            void setThresholdChanged(const double threshold);
            void setLambdaChanged(const double lambda);
            void setMChanged(const double m);
            void setNChanged(const double n);
            void setInfluenceChanged(const double influence);
            void setAsymmetryChanged(const double asymmetry);
            void setLodValueChanged(int value);
            void setComputeR();
            void setLoadDistribution(std::string filePath);
            void setDisplayProximities();
            void setDisplayDistribution();
            void setDisplayWeight();
            void setComputeClusters();
            void setDisplayClusters();
            int getThresholdCluster();
            void setNbClustersToComputeChanged(const int nb);
            void setNbClustersToDisplayChanged(const int nb);
            void setConstructM0();
            void setThresholdValueChanged(int value);
            int getThresholdMax();
            void setAsymmetryValueChanged(int value);
            int getAsymmetryMax();

            int getNbFacesMax();
            void computeNbFacesMax();
            void computeNbFacesMax2();

            void addMesh(MeshContactElement* mesh, const std::string& entityName, const std::string& componentName);

            void computeThreshold();
            void computeThresholdTest();
            void distanceDistribution();
            void compareDistanceDistribution();
            void distanceAsymmetryDistribution();
            void loadDistribution(std::string filePath);
            void sortDistAsymm();
            void displayDistribution(Scalar distValue, Scalar asymmValue);
            void distanceAsymmetryFiles();
            void distanceAsymmetryFile();
            void distanceAsymmetryFile2(); // the asymmetry is normalized
            void computeFacesArea();
            void weightedDistanceFile();
            void computeFacesAsymmetry();
            void finalDistanceFile();
            void finalDistribCleaned(); // keeping only minima and maxima
            void finalDistanceFile2(); // with distance function
            void thresholdComputation();

            void kmeans(int k);
            Scalar silhouette();
            void clustering(Scalar silhouetteMin, int nbClustersMax);

            void findClusters();
            void colorClusters();
            void findClusters2(); // same with distance function
            void colorClusters2();

            void topologicalPersistence();
            //int nbClusters();
            void colorClusters3();

            void normalize();
            void scale(Scalar n);

            bool edgeErrorComputation(Ra::Core::HalfEdge_ptr h, int objIndex, Scalar& error, Ra::Core::Vector3& p);
            void constructPriorityQueues2();
            void updatePriorityQueue2(Ra::Core::Index vsIndex, Ra::Core::Index vtIndex, int objIndex);
            bool edgeCollapse(int objIndex);

        private:

            int m_nb_faces_max;
            int m_nbfacesinit;
            int m_nbfaces;
            int m_nbobjects; // number of objects to be simplified (the first ones to be loaded into the scene)
            Scalar m_threshold; // distance used to define "contacts"
            Scalar m_influence; // weight value for the threshold in order to compute a broader threshold
            Scalar m_asymmetry; // threshold for the asymmetry between the 2 triangles defining a contact
            Scalar m_broader_threshold; // amplified threshold in order to have a significant weight up until m_threshold
            Scalar m_lambda; // influence of the original quadric considering contacts
            Scalar m_m;
            Scalar m_n; // slope of the weight function for contacts

            Eigen::Matrix<Scalar, NBMAX_ELEMENTS, NBMAX_ELEMENTS> m_thresholds; // thresholds for each pair of objects
            std::vector<std::vector<std::vector<std::pair<Ra::Core::Index,Scalar> > > > m_distances; // distances for each pair of objects
            std::vector<std::vector<Scalar> > m_facesArea; // area of the faces of each object
            std::vector<std::vector<Scalar> > m_facesAsymmetry; // asymmetry ponderation of the faces of each object

            struct comparePtDistribByDistance
            {
                inline bool operator() (const PtDistrib &p1, const PtDistrib &p2) const
                {
                    return p1.r <= p2.r;
                }
            };
            typedef std::set<PtDistrib, comparePtDistribByDistance> DistanceSorting;
            DistanceSorting m_distSort;

            struct comparePtDistribByAsymmetry
            {
                inline bool operator() (const PtDistrib &p1, const PtDistrib &p2) const
                {
                    return p1.a < p2.a;
                }
            };
            typedef std::set<PtDistrib, comparePtDistribByAsymmetry> AsymmetrySorting;
            AsymmetrySorting m_asymmSort;

            std::vector<PtDistrib> m_distrib;
            std::vector<std::pair<Scalar, std::vector<int> > > m_clusters; // position of cluster centers and indices of cluster faces in m_distrib

            std::vector<std::pair<Scalar,Scalar> > m_finalDistrib; // distance and asymmetry positions of the last distribution until the asymmetry is equal to 0
            std::vector<std::pair<Scalar,Scalar> > m_finalDistribCleaned; // keeping only minima and maxima to determine the max nb of clusters
            std::vector<Scalar> m_finalClusters; // distances defining the clusters
            std::vector<std::pair<Scalar,Scalar> > m_finalDistrib2; // same with distance function
            std::vector<Scalar> m_finalClusters2;
            std::vector<std::pair<Scalar,Scalar> > m_finalDistrib3; // same with topological persistence
            std::vector<Scalar> m_finalClusters3;

            int m_nbclusters_compute;
            int m_nbclusters_display;

            struct compareMinByDescendingOrdinate
            {
                inline bool operator() (const std::pair<Scalar,Scalar> &p1, const std::pair<Scalar,Scalar> &p2) const
                {
                    return p1.second >= p2.second;
                }
            };
            typedef std::set<std::pair<Scalar,Scalar>, compareMinByDescendingOrdinate> MinSorting;
            MinSorting m_minSort;

            struct compareMaxByAscendingAbscissa
            {
                inline bool operator() (const std::pair<Scalar,Scalar>  &p1, const std::pair<Scalar,Scalar> &p2) const
                {
                    return p1.first <= p2.first;
                }
            };
            typedef std::set<std::pair<Scalar,Scalar>, compareMaxByAscendingAbscissa> MaxSorting;
            MaxSorting m_maxSort;

            struct compareDiffByDescendingValue
            {
                inline bool operator() (const std::pair<std::pair<Scalar,Scalar>,std::pair<Scalar,Scalar> > &d1, const std::pair<std::pair<Scalar,Scalar>,std::pair<Scalar,Scalar> > &d2) const
                {
                    return std::abs(d1.first.second - d1.second.second) > std::abs(d2.first.second - d2.second.second);
                }
            };
            typedef std::set<std::pair<std::pair<Scalar,Scalar>,std::pair<Scalar,Scalar> >, compareDiffByDescendingValue> DiffSorting;
            DiffSorting m_diffSort;

            struct comparePlotByAscendingDistance
            {
                inline bool operator() (const std::pair<Scalar,Scalar> &p1, const std::pair<Scalar,Scalar> &p2) const
                {
                    return p1.first < p2.first;
                }
            };
            typedef std::set<std::pair<Scalar,Scalar>, comparePlotByAscendingDistance> PlotSorting;
            PlotSorting m_plotSort;

            Scalar m_threshold_max;
            Scalar m_asymmetry_max;

            Scalar m_asymmetry_mean;
            Scalar m_asymmetry_median;
            Scalar m_distance_median;

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
