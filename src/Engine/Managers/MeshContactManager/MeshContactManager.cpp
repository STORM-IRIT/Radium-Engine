#include <Engine/Managers/MeshContactManager/MeshContactManager.hpp>

#include <string>
#include <iostream>

#include <Core/Tasks/TaskQueue.hpp>
#include <Core/File/FileData.hpp>
#include <Core/File/HandleData.hpp>

#include <Engine/RadiumEngine.hpp>

#include <Engine/Entity/Entity.hpp>

#include "Eigen/Core"

#include <Core/Geometry/Normal/Normal.hpp>

#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>

#include <iostream>
#include <fstream>

#include<chrono>

namespace Ra
{
    namespace Engine
    {

        MeshContactManager::MeshContactManager()
            :m_nb_faces_max( 0 )
            ,m_nbfacesinit( 0 )
            ,m_nbfaces( 0 )
            ,m_nbobjects( 0 )
            ,m_threshold( 0.0 )
            ,m_broader_threshold ( 0.0 )
            ,m_lambda( 0.0 )
            ,m_m( 3.0 )
            ,m_n ( 2.0 )
            ,m_influence ( 0.9 )
            ,m_asymmetry ( 0.0 )
            ,m_curr_vsplit( 0 )
            ,m_threshold_max( 0 )
            ,m_asymmetry_max( 0 )
            ,m_asymmetry_mean( 0 )
            ,m_asymmetry_median( 0 )
            ,m_distance_median( 0 )
            ,m_nbclusters_compute( 4 )
            ,m_nbclusters_display( 1 )
            ,m_proximity( true )
            ,m_weight( 1 )
            ,m_boundary( false )
            ,m_pqueue_time( 0 )
        {
        }

        void MeshContactManager::setNbFacesChanged(const int nb)
        {
            m_nbfacesinit = nb;
        }

        void MeshContactManager::setNbObjectsChanged(const int nb)
        {
            m_nbobjects = nb;
        }

        void MeshContactManager::computeNbFacesMax()
        {
            m_nb_faces_max = 0;

            for (const auto& elem : m_meshContactElements)
            {
                m_nb_faces_max += static_cast<MeshContactElement*>(elem)->getNbFacesMax();
            }
        }

        void MeshContactManager::computeNbFacesMax2()
        {
            m_nb_faces_max = 0;

            for (uint i = 0; i < m_initTriangleMeshes.size(); i++)
            {
                m_nb_faces_max += m_initTriangleMeshes[i].m_triangles.size();
            }
        }

        void MeshContactManager::setThresholdChanged(const double threshold)
        {
            m_threshold = threshold;
        }

        void MeshContactManager::setLambdaChanged(const double lambda)
        {
            m_lambda = lambda;
        }

        void MeshContactManager::setMChanged(const double m)
        {
            m_m = m;
        }

        void MeshContactManager::setNChanged(const double n)
        {
            m_n = n;
        }

        void MeshContactManager::setInfluenceChanged(const double influence)
        {
            m_influence = influence;
        }

        void MeshContactManager::setAsymmetryChanged(const double asymmetry)
        {
            m_asymmetry = asymmetry;
        }

        void MeshContactManager::setNbClustersToComputeChanged(const int nb)
        {
            m_nbclusters_compute = nb;
        }

        void MeshContactManager::setNbClustersToDisplayChanged(const int nb)
        {
            m_nbclusters_display = nb;
        }

        void MeshContactManager::addMesh(MeshContactElement* mesh, const std::string& entityName, const std::string& componentName)
        {
            m_meshContactElements.push_back(mesh);

            //mesh->computeTriangleMesh();
            mesh->computeMesh(entityName,componentName);

            m_initTriangleMeshes.push_back(mesh->getInitTriangleMesh());

            Super4PCS::TriangleKdTree<>* trianglekdtree = new Super4PCS::TriangleKdTree<>();
            m_trianglekdtrees.push_back(trianglekdtree);
            m_trianglekdtrees[m_trianglekdtrees.size()-1] = mesh->computeTriangleKdTree(m_initTriangleMeshes[m_initTriangleMeshes.size()-1]);
            LOG(logINFO) << "m_trianglekdtrees size : " << m_trianglekdtrees.size();

            m_nbobjects++;
        }

        void MeshContactManager::computeThreshold()
        {
            m_thresholds.setZero();
            Scalar min_th = std::numeric_limits<Scalar>::max();
            Scalar min_th_obj;
            Scalar th;
            std::pair<Ra::Core::Index,Scalar> triangle;
            for (uint i = 0; i < m_meshContactElements.size(); i++)
            {
                MeshContactElement* obj1 = m_meshContactElements[i];
                Ra::Core::VectorArray<Ra::Core::Triangle> t1 = obj1->getInitTriangleMesh().m_triangles;
                Ra::Core::VectorArray<Ra::Core::Vector3> v1 = obj1->getInitTriangleMesh().m_vertices;
                for (uint j = i + 1; j < m_meshContactElements.size(); j++)
                {
                    min_th_obj = std::numeric_limits<Scalar>::max();
                    // for each face of an object, we find the closest face in the other object
                    for (uint k = 0; k < t1.size(); k++)
                    {
                        triangle = m_trianglekdtrees[j]->doQueryRestrictedClosestIndexTriangle(v1[t1[k][0]],v1[t1[k][1]],v1[t1[k][2]]);
                        CORE_ASSERT(triangle.first > -1, "Invalid triangle index.");
                        th = triangle.second;
                        if (th < min_th_obj)
                        {
                            min_th_obj = th;
                        }
                    }
                    m_thresholds(i,j) = min_th_obj;
                    if (min_th_obj < min_th)
                    {
                        min_th = min_th_obj;
                    }
                }
            }
            m_broader_threshold = min_th;
        }

        void MeshContactManager::computeThresholdTest()
        {
            Scalar min_th = std::numeric_limits<Scalar>::max();
            Scalar th;
            for (uint i = 0; i < m_meshContactElements.size(); i++)
            {
                MeshContactElement* obj1 = m_meshContactElements[i];
                Ra::Core::VectorArray<Ra::Core::Triangle> t1 = obj1->getInitTriangleMesh().m_triangles;
                Ra::Core::VectorArray<Ra::Core::Vector3> v1 = obj1->getInitTriangleMesh().m_vertices;
                for (uint j = i + 1; j < m_meshContactElements.size(); j++)
                {
                    MeshContactElement* obj2 = m_meshContactElements[j];
                    Ra::Core::VectorArray<Ra::Core::Triangle> t2 = obj2->getInitTriangleMesh().m_triangles;
                    Ra::Core::VectorArray<Ra::Core::Vector3> v2 = obj2->getInitTriangleMesh().m_vertices;
                    // for each face of an object, we find the closest face in the other object
                    for (uint k = 0; k < t1.size(); k++)
                    {
                        for (uint l = 0; l < t2.size(); l++)
                        {
                            const Ra::Core::Vector3 triangle1[3] = {v1[t1[k][0]], v1[t1[k][1]], v1[t1[k][2]]};
                            const Ra::Core::Vector3 triangle2[3] = {v2[t2[l][0]], v2[t2[l][1]], v2[t2[l][2]]};
                            th = Ra::Core::DistanceQueries::triangleToTriSq(triangle1,triangle2).distance;
                            if (th < min_th)
                            {
                                min_th = th;
                            }
                        }
                    }
                }
            }
            LOG(logINFO) << "Test threshold computed value : " << min_th;
        }

        void MeshContactManager::distanceDistribution()
        {
            Scalar dist;
            Scalar step;
            Scalar th;
            std::pair<Ra::Core::Index,Scalar> triangle;
            for (uint i = 0; i < m_meshContactElements.size(); i++)
            {
                MeshContactElement* obj1 = m_meshContactElements[i];
                Ra::Core::VectorArray<Ra::Core::Triangle> t1 = obj1->getInitTriangleMesh().m_triangles;
                Ra::Core::VectorArray<Ra::Core::Vector3> v1 = obj1->getInitTriangleMesh().m_vertices;
                Super4PCS::AABB3D aabb1 = Super4PCS::AABB3D();
                for (uint a = 0; a < v1.size(); a++)
                {
                    aabb1.extendTo(v1[a]);
                }
                for (uint j = 0; j < m_meshContactElements.size(); j++)
                {
                    if (j != i)
                    {
                        MeshContactElement* obj2 = m_meshContactElements[j];
                        Ra::Core::VectorArray<Ra::Core::Vector3> v2 = obj2->getInitTriangleMesh().m_vertices;
                        Super4PCS::AABB3D aabb2 = Super4PCS::AABB3D();
                        for (uint b = 0; b < v2.size(); b++)
                        {
                            aabb2.extendTo(v2[b]);
                        }
                        dist = (aabb1.center() - aabb2.center()).norm() / 2;
                        step = dist / NBMAX_STEP;
                        uint distArray[NBMAX_STEP] = {0};
                        uint first, last, mid;

                        // for each face of an object, we find the closest face in the other object
                        for (uint k = 0; k < t1.size(); k++)
                        {
                            triangle = m_trianglekdtrees[j]->doQueryRestrictedClosestIndexTriangle(v1[t1[k][0]],v1[t1[k][1]],v1[t1[k][2]]);
                            CORE_ASSERT(triangle.first > -1, "Invalid triangle index.");
                            th = triangle.second;

                            first = 0;
                            last = NBMAX_STEP;
                            mid = NBMAX_STEP / 2;
                            while ((first + 1) < last)
                            {
                                if (th <= mid * step)
                                {
                                    last = mid;
                                    mid = (first + last) / 2;
                                }
                                else
                                {
                                    first = mid;
                                    mid = (first + last) / 2;
                                }
                            }
                            if (last == NBMAX_STEP)
                            {
                                if (th <= NBMAX_STEP * step)
                                    distArray[first]++;
                            }
                            else
                            {
                                distArray[first]++;
                            }
                        }

                        std::ofstream file("Dist_" + std::to_string(i) + "_" + std::to_string(j) + ".txt", std::ios::out | std::ios::trunc);
                        CORE_ASSERT(file, "Error while opening distance distribution file.");
                        for (uint k = 0; k < NBMAX_STEP; k++)
                        {
                           file << step * (k + 1)<< " " << distArray[k] << std::endl;
                        }
                        file.close();
                    }
                }
            }
        }

        void MeshContactManager::compareDistanceDistribution()
        {
            Scalar step;
            int nb1, nb2;

            for (uint i = 0; i < m_meshContactElements.size(); i++)
            {
                for (uint j = i+1; j < m_meshContactElements.size(); j++)
                {
                        std::ifstream file1("Dist_" + std::to_string(i) + "_" + std::to_string(j) + ".txt", std::ios::in);
                        std::ifstream file2("Dist_" + std::to_string(j) + "_" + std::to_string(i) + ".txt", std::ios::in);
                        CORE_ASSERT(file1 && file2, "Error while opening distance distribution files.");

                        std::ofstream file3("Diff_" + std::to_string(i) + "_" + std::to_string(j) + ".txt", std::ios::out | std::ios::trunc);
                        CORE_ASSERT(file3, "Error while opening comparaison distance distribution file.");
                        while (!file1.eof())
                        {
                            file1 >> step >> nb1;
                            file2 >> step >> nb2;
                            file3 << step << " " << abs(nb2-nb1) << std::endl;
                        }
                        file1.close();
                        file2.close();
                        file3.close();
                }
            }
        }

        void MeshContactManager::computeAABB()
        {
            for (auto& obj : m_meshContactElements)
            {
                Super4PCS::AABB3D aabb = Super4PCS::AABB3D();
                Ra::Core::VectorArray<Ra::Core::Vector3> vertices = obj->getInitTriangleMesh().m_vertices;
                for (auto& v: vertices)
                {
                    aabb.extendTo(v);
                    m_aabb_scene.extendTo(v);
                }
                m_aabb.push_back(aabb);
            }

            LOG(logINFO) << "Scene AABB diagonal : " << m_aabb_scene.diagonal();
        }

        bool MeshContactManager::intersectionAABB(int id1, int id2)
        {
            int i = 0;

            Scalar epsilon = m_aabb_scene.diagonal() / 1000;

            while (i < 3)
            {
                if (m_aabb[id1].max()[i] + epsilon < m_aabb[id2].min()[i] || m_aabb[id2].max()[i] + epsilon < m_aabb[id1].min()[i])
                {
                    return false;
                }
                i++;
            }
            return true;
        }

        void MeshContactManager::proximityPairs()
        {
            //m_proximityPairs.setZero();
            m_proximityPairs.resize(m_meshContactElements.size(),m_meshContactElements.size());
            m_proximityPairs.setZero();

            for (uint i = 0; i < m_meshContactElements.size(); i++)
            {
                for (uint j = i + 1; j < m_meshContactElements.size(); j++)
                {
                    if (intersectionAABB(i,j))
                    {
                        m_proximityPairs(i,j) = 1;
                        m_proximityPairs(j,i) = 1;
                    }
                }
            }

            for (uint i = 0; i < m_meshContactElements.size(); i++)
            {
                for (uint j = i + 1; j < m_meshContactElements.size(); j++)
                {
                    LOG(logINFO) << i + 1 << " and " << j + 1 << " proximity : " << m_proximityPairs(i,j);
                }
            }
        }

        bool MeshContactManager::intersectionAABB2(int id1, int id2)
        {
            int i = 0;

            Scalar epsilon = m_broader_threshold;

            while (i < 3)
            {
                if (m_aabb[id1].max()[i] + epsilon < m_aabb[id2].min()[i] || m_aabb[id2].max()[i] + epsilon < m_aabb[id1].min()[i])
                {
                    return false;
                }
                i++;
            }
            return true;
        }

        void MeshContactManager::proximityPairs2()
        {
            //m_proximityPairs2.setZero();
            m_proximityPairs2.resize(m_meshContactElements.size(),m_meshContactElements.size());
            m_proximityPairs2.setZero();

            for (uint i = 0; i < m_meshContactElements.size(); i++)
            {
                for (uint j = i + 1; j < m_meshContactElements.size(); j++)
                {
                    if (intersectionAABB2(i,j))
                    {
                        m_proximityPairs2(i,j) = 1;
                        m_proximityPairs2(j,i) = 1;
                    }
                }
            }

            for (uint i = 0; i < m_meshContactElements.size(); i++)
            {
                for (uint j = i + 1; j < m_meshContactElements.size(); j++)
                {
                    LOG(logINFO) << i + 1 << " and " << j + 1 << " proximity : " << m_proximityPairs2(i,j);
                }
            }
        }

        // Data to be loaded next time around
        void MeshContactManager::distanceAsymmetryDistribution()
        {
            std::ofstream file("Data.txt", std::ios::out | std::ios::trunc);
            CORE_ASSERT(file, "Error while opening distance asymmetry distribution file.");

            std::pair<Ra::Core::Index,Scalar> triangle;

            for (uint i = 0; i < m_meshContactElements.size(); i++)
            {
                MeshContactElement* obj1 = m_meshContactElements[i];
                Ra::Core::VectorArray<Ra::Core::Triangle> t1 = obj1->getInitTriangleMesh().m_triangles;
                Ra::Core::VectorArray<Ra::Core::Vector3> v1 = obj1->getInitTriangleMesh().m_vertices;


                std::vector<std::vector<std::pair<Ra::Core::Index,Scalar> > > obj1_distances;

                for (uint j = 0; j < m_meshContactElements.size(); j++)
                {

                    std::vector<std::pair<Ra::Core::Index,Scalar> > distances;

                    if (m_proximityPairs(i,j))
                    {

                            // for each face of an object, we find the closest face in the other object
                            for (uint k = 0; k < t1.size(); k++)
                            {
                                triangle = m_trianglekdtrees[j]->doQueryRestrictedClosestIndexTriangle(v1[t1[k][0]],v1[t1[k][1]],v1[t1[k][2]]);
                                CORE_ASSERT(triangle.first > -1, "Invalid triangle index.");

                                distances.push_back(triangle);

                                file << triangle.first << " " << triangle.second << std::endl;
                            }

//                        }
                    }

                    obj1_distances.push_back(distances);
                }
                m_distances.push_back(obj1_distances);
            }
            file.close();

            sortDistAsymm(); // to move in case there are more than 2 objects and it's not needed
        }

        void MeshContactManager::loadDistribution(std::string filePath)
        {
            std::ifstream file(filePath, std::ios::in);
            CORE_ASSERT(file, "Error while opening distance asymmetry distributions file.");

//            if (filePath != "")
//            {
                int id;
                Scalar dist;

                std::pair<Ra::Core::Index,Scalar> triangle;

                for (uint i = 0; i < m_meshContactElements.size(); i++)
                {
                    std::vector<std::vector<std::pair<Ra::Core::Index,Scalar> > > obj_distances;

                    int nbFaces = m_meshContactElements[i]->getInitTriangleMesh().m_triangles.size();

                    for (uint j = 0; j < m_meshContactElements.size(); j++)
                    {
                        std::vector<std::pair<Ra::Core::Index,Scalar> > distances;

                        if (m_proximityPairs(i,j))
                        {
                            for (uint k = 0; k < nbFaces; k++)
                            {
                                file >> id >> dist;
                                triangle.first = id;
                                triangle.second = dist;
                                distances.push_back(triangle);
                            }

                        }

                        obj_distances.push_back(distances);
                    }

                    m_distances.push_back(obj_distances);
                }
//            }

                sortDistAsymm(); // to move in case there are more than 2 objects and it's not needed
        }

        void MeshContactManager::sortDistAsymm()
        {
            for (uint i = 0; i < m_meshContactElements.size(); i++)
            {
                int nbFaces = m_meshContactElements[i]->getInitTriangleMesh().m_triangles.size();

                for (uint j = 0; j < m_meshContactElements.size(); j++)
                {
                    if (m_proximityPairs(i,j))
                    {
                        for (uint k = 0; k < nbFaces; k++)
                        {
                            PtDistrib pt;
                            pt.objId = i;
                            pt.otherObjId = j;
                            pt.faceId = k;
                            pt.r = m_distances[i][j][k].second;
                            pt.a = abs(m_distances[i][j][k].second - m_distances[j][i][m_distances[i][j][k].first].second);
                            m_distrib.push_back(pt);
                            m_distSort.insert(pt);
                            m_asymmSort.insert(pt);
                            if (pt.r > m_threshold_max)
                            {
                                m_threshold_max = pt.r;
                            }
                            if (pt.a > m_asymmetry_max)
                            {
                                m_asymmetry_max = pt.a;
                            }
                        }
                    }
                }
            }

            LOG(logINFO) << "Threshold max : " << m_threshold_max;
            LOG(logINFO) << "Asymmetry max : " << m_asymmetry_max;
        }

        // value that will be displayed as threshold in UI
        int MeshContactManager::getThresholdCluster()
        {
            return ((int)(m_finalClusters3[m_nbclusters_display - 1] * PRECISION) + 1);
        }

        // value that will be used for the slider in UI
        int MeshContactManager::getThresholdMax()
        {
            return ((int)(m_threshold_max * PRECISION) + 1);
        }

        // value that will be used for the slider in UI
        int MeshContactManager::getAsymmetryMax()
        {
           return ((int)(m_asymmetry_max * PRECISION) + 1);
        }

        // works well in case of 2 objects only
        void MeshContactManager::displayDistribution(Scalar distValue, Scalar asymmValue)
        {
            //Ra::Core::Vector4 color(0, 0, 1, 0);

            Ra::Core::Vector4 distColor(1, 0, 0, 0);
            Ra::Core::Vector4 asymmColor(0, 1, 0, 0);

            Ra::Core::Vector4 vertexColor (0, 0, 0, 0);
            for (uint i = 0; i < m_meshContactElements.size(); i++)
            {
                MeshContactElement* obj = m_meshContactElements[i];
                int nbVertices = obj->getMesh()->getGeometry().m_vertices.size();
                Ra::Core::Vector4Array colors;
                for (uint v = 0; v < nbVertices; v++)
                {
                    colors.push_back(vertexColor);
                }
                obj->getMesh()->addData(Ra::Engine::Mesh::VERTEX_COLOR, colors);
            }

            for (uint j = 0; j < m_distrib.size(); j++)
            {
                if (m_distrib[j].r <= distValue && m_distrib[j].a <= asymmValue)
                {
                    MeshContactElement* obj = m_meshContactElements[m_distrib[j].objId];
                    //Ra::Core::VectorArray<Ra::Core::Triangle> t = obj->getInitTriangleMesh().m_triangles;
                    Ra::Core::VectorArray<Ra::Core::Triangle> t = obj->getTriangleMeshDuplicate().m_triangles;
                    Ra::Core::Vector4Array colors = obj->getMesh()->getData(Ra::Engine::Mesh::VERTEX_COLOR);

                    Scalar distCoeff, asymmCoeff;
                    if (distValue != 0)
                    {
                        distCoeff = (distValue - m_distrib[j].r) / distValue;
                    }
                    else
                    {
                        distCoeff = 1;
                    }
                    if (asymmValue != 0)
                    {
                        asymmCoeff = (asymmValue - m_distrib[j].a) / asymmValue;
                    }
                    else
                    {
                        asymmCoeff = 1;
                    }

                    colors[t[m_distrib[j].faceId][0]] = distCoeff * distColor + asymmCoeff * asymmColor;
                    colors[t[m_distrib[j].faceId][1]] = distCoeff * distColor + asymmCoeff * asymmColor;
                    colors[t[m_distrib[j].faceId][2]] = distCoeff * distColor + asymmCoeff * asymmColor;
                    obj->getMesh()->addData(Ra::Engine::Mesh::VERTEX_COLOR, colors);
                }
            }
        }

        // Displaying colored faces in light of r and a
        void MeshContactManager::setDisplayDistribution()
        {
            //sortDistAsymm();

            // reloading initial mesh in case of successive simplifications
            for (uint objIndex = 0; objIndex < m_meshContactElements.size(); objIndex++)
            {
                m_meshContactElements[objIndex]->setMesh(m_meshContactElements[objIndex]->getTriangleMeshDuplicate());
            }

            displayDistribution(m_threshold, m_asymmetry);
        }

        void MeshContactManager::setDisplayWeight()
        {
            if (m_aabb.size() == 0)
            {
                computeAABB();
            }

//            if (m_proximityPairs.size() == 0)
//            {
//                proximityPairs();
//            }

            thresholdComputation(); // computing m_broader_threshold

            proximityPairs2();

            // reloading initial mesh in case of successive simplifications
            for (uint objIndex = 0; objIndex < m_meshContactElements.size(); objIndex++)
            {
                m_meshContactElements[objIndex]->setMesh(m_meshContactElements[objIndex]->getTriangleMeshDuplicate());
            }

            Ra::Core::Vector4 vertexColor (0, 0, 0, 0);
            for (uint i = 0; i < m_meshContactElements.size(); i++)
            {
                MeshContactElement* obj = m_meshContactElements[i];
                int nbVertices = obj->getMesh()->getGeometry().m_vertices.size();
                Ra::Core::Vector4Array colors;
                for (uint v = 0; v < nbVertices; v++)
                {
                    colors.push_back(vertexColor);
                }
                obj->getMesh()->addData(Ra::Engine::Mesh::VERTEX_COLOR, colors);
            }

            Ra::Core::Vector4 displayColors[5] = {{1,0,0,0}, {1,0.5,0,0}, {1,1,0,0}, {0,1,0,0}, {0,0,1,0}};
            //Ra::Core::Vector4 displayColors[5] = {{0.45,0,0,0}, {0.86,0.38,0.15,0}, {0.86,0.64,0.15,0}, {0.10,0.43,0.24,0}, {0.02,0.16,0.25,0}};

            DistanceSorting::iterator it = m_distSort.begin();

            Scalar step = 0.2;

            uint i = 1;

            LOG(logINFO) << "Broader threshold : " << m_broader_threshold;

            Scalar weight = std::pow(std::pow((*it).r/m_broader_threshold, m_m) - 1, m_n);

            while (it != m_distSort.end() && i <= 5)
            {
                while (it != m_distSort.end() && (*it).r <= m_broader_threshold && weight >= (1- step * i))
                {
                    MeshContactElement* obj = m_meshContactElements[(*it).objId];
                    Ra::Core::VectorArray<Ra::Core::Triangle> t = obj->getTriangleMeshDuplicate().m_triangles;
                    Ra::Core::Vector4Array colors = obj->getMesh()->getData(Ra::Engine::Mesh::VERTEX_COLOR);

                    colors[t[(*it).faceId][0]] = displayColors[i-1];
                    colors[t[(*it).faceId][1]] = displayColors[i-1];
                    colors[t[(*it).faceId][2]] = displayColors[i-1];

                    obj->getMesh()->addData(Ra::Engine::Mesh::VERTEX_COLOR, colors);

                    std::advance(it,1);
                    if (it != m_distSort.end())
                    {
                        weight = std::pow(std::pow((*it).r/m_broader_threshold, m_m) - 1, m_n);
                    }
                }
                i++;
            }
        }

        void MeshContactManager::distanceAsymmetryFiles()
        {
            Scalar dist, asymm;

            // for each object
            for (uint i = 0; i < m_distances.size(); i++)
            {
                // for each other object whose bbox intersects the one of the object
                for (uint j = i + 1; j < m_distances[i].size(); j++)
                {
                    std::ofstream file("Distrib_" + std::to_string(i) + "_" + std::to_string(j) + ".txt", std::ios::out | std::ios::trunc);
                    CORE_ASSERT(file, "Error while opening distance distribution file.");

                    // for each face of the object the closest face in the other object is found
                    for (uint k = 0; k < m_distances[i][j].size(); k++)
                    {
                        std::pair<Ra::Core::Index,Scalar> triangle1 = m_distances[i][j][k];

                        std::pair<Ra::Core::Index,Scalar> triangle2 = m_distances[j][i][triangle1.first];
                        dist = (triangle1.second + triangle2.second) / 2;
                        asymm = abs(triangle1.second - triangle2.second);
                        file << dist << " " << asymm << std::endl;
                    }

                    // for each face of the other object the closest face in the object is found
                    for (uint k = 0; k < m_distances[j][i].size(); k++)
                    {
                        std::pair<Ra::Core::Index,Scalar> triangle1 = m_distances[j][i][k];

                        std::pair<Ra::Core::Index,Scalar> triangle2 = m_distances[i][j][triangle1.first];
                        dist = (triangle1.second + triangle2.second) / 2;
                        asymm = abs(triangle1.second - triangle2.second);
                        file << dist << " " << asymm << std::endl;
                    }

                    file.close();
                }
            }
        }

        void MeshContactManager::distanceAsymmetryFile()
        {
            Scalar dist, asymm;

            std::ofstream file("Distrib.txt", std::ios::out | std::ios::trunc);
            CORE_ASSERT(file, "Error while opening distance distribution file.");

            for (uint i = 0; i < m_distances.size(); i++)
            {
                for (uint j = 0; j < m_distances[i].size(); j++)
                {
                    for (uint k = 0; k < m_distances[i][j].size(); k++)
                    {
                        dist = m_distances[i][j][k].second;
                        asymm = abs(m_distances[i][j][k].second - m_distances[j][i][m_distances[i][j][k].first].second);
                        file << dist << " " << asymm << std::endl;
                    }
                }
            }

            file.close();
        }

        // Normalized asymmetry
        void MeshContactManager::distanceAsymmetryFile2()
        {
            Scalar dist, asymm;

            std::ofstream file("Distrib.txt", std::ios::out | std::ios::trunc);
            CORE_ASSERT(file, "Error while opening distance distribution file.");

            for (uint i = 0; i < m_distances.size(); i++)
            {
                for (uint j = 0; j < m_distances[i].size(); j++)
                {
                    for (uint k = 0; k < m_distances[i][j].size(); k++)
                    {
                        dist = m_distances[i][j][k].second;
                        asymm = abs(m_distances[i][j][k].second - m_distances[j][i][m_distances[i][j][k].first].second);
                        asymm = asymm / m_asymmetry_max;
                        file << dist << " " << asymm << std::endl;
                    }
                }
            }

            file.close();
        }

        void MeshContactManager::computeFacesArea()
        {
            for (uint i = 0; i < m_meshContactElements.size(); i++)
            {
                std::vector<Scalar> objFacesArea;
                MeshContactElement* obj = m_meshContactElements[i];
                Ra::Core::VectorArray<Ra::Core::Triangle> t = obj->getInitTriangleMesh().m_triangles;
                Ra::Core::VectorArray<Ra::Core::Vector3> v = obj->getInitTriangleMesh().m_vertices;
                for (uint j = 0; j < t.size(); j++)
                {
                    Scalar area = Ra::Core::Geometry::triangleArea(v[t[j][0]],v[t[j][1]],v[t[j][2]]);
                    objFacesArea.push_back(area);
                }
                m_facesArea.push_back(objFacesArea);
            }
        }

        void MeshContactManager::weightedDistanceFile()
        {
            Scalar dist, area;
            Scalar step = m_threshold_max / NBMAX_STEP;

            Scalar areas[NBMAX_STEP] = {0};

            for (uint i = 0; i < m_distrib.size(); i++)
            {
                dist = m_distrib[i].r;
                area = m_facesArea[m_distrib[i].objId][m_distrib[i].faceId];

                int slot = std::floor(dist / step);
                if (slot == NBMAX_STEP)
                {
                    slot--;
                }
                areas[slot] = areas[slot] + area;
            }

            std::ofstream file("Weighted_distrib.txt", std::ios::out | std::ios::trunc);
            CORE_ASSERT(file, "Error while opening weighted distance distribution file.");

            for (uint i = 0; i < NBMAX_STEP; i++)
            {
                //file << ((2 * i + 1) * step) / 2 << " " << areas[i] << std::endl;
                file << (i + 1) * step << " " << areas[i] << std::endl;
            }

            file.close();
        }

        void MeshContactManager::computeFacesAsymmetry()
        {
            int objId = m_distrib[0].objId;
            uint i = 0;
            while (objId != -1)
            {
                std::vector<Scalar> asymm;
                while (m_distrib[i].objId == objId)
                {
                    asymm.push_back(m_distrib[i].a);
                    i++;
                }
                m_facesAsymmetry.push_back(asymm);
                if (i < m_distrib.size())
                {
                    objId = m_distrib[i].objId;
                }
                else
                {
                    objId = -1;
                }
            }
        }

        void MeshContactManager::finalDistanceFile()
        {
            Scalar dist, area, asymm;
            Scalar step = m_threshold_max / NBMAX_STEP;

            Scalar areas[NBMAX_STEP] = {0};

            // computing the mean asymmetry value
//            for (uint j = 0; j < m_distrib.size(); j++)
//            {
//                m_asymmetry_mean += m_distrib[j].a;
//            }
//            m_asymmetry_mean /= m_distrib.size();

            // computing the median asymmetry value
            AsymmetrySorting::iterator it = m_asymmSort.begin();

            if (m_asymmSort.size() % 4 == 0 || m_asymmSort.size() % 4 == 1)
            {
                std::advance(it, m_asymmSort.size() / 4);
                Scalar m = (*it).a;
                std::advance(it, 1);
                m_asymmetry_median = (m + (*it).a) / 2;
            }
            else
            {
                std::advance(it, m_asymmSort.size() / 4 + 1);
                m_asymmetry_median = (*it).a;
            }

            //LOG(logINFO) << "Asymmetry mean : " << m_asymmetry_mean;
            LOG(logINFO) << "Asymmetry median : " << m_asymmetry_median;

            for (uint i = 0; i < m_distrib.size(); i++)
            {
                dist = m_distrib[i].r;

                area = m_facesArea[m_distrib[i].objId][m_distrib[i].faceId];
                asymm = m_facesAsymmetry[m_distrib[i].objId][m_distrib[i].faceId];
                if (asymm > m_asymmetry_median)
                {
                    asymm = 0;
                }
                else
                {
                    //asymm = 1 - (asymm / m_asymmetry_median);
                    asymm = std::pow(1 - std::pow((asymm / m_asymmetry_median),2),2); // weight function for anisotropy
                }

                int slot = std::floor(dist / step);
                if (slot == NBMAX_STEP)
                {
                    slot--;
                }
                areas[slot] = areas[slot] + area * asymm;
            }

            std::ofstream file("Final_distrib.txt", std::ios::out | std::ios::trunc);
            CORE_ASSERT(file, "Error while opening final distance distribution file.");


            m_finalDistrib.emplace_back(0,0);

            for (uint i = 0; i < NBMAX_STEP; i++)
            {
                //file << ((2 * i + 1) * step) / 2 << " " << areas[i] << std::endl;
                file << (i + 1) * step << " " << areas[i] << std::endl;

//                if (areas[i] != 0)
//                {
                    std::pair<Scalar,Scalar> p;
                    //p.first = ((2 * i + 1) * step) / 2;
                    p.first = (i + 1) * step;
                    p.second = areas[i];
                    m_finalDistrib.push_back(p);
//                }
            }

            file.close();
        }

        void MeshContactManager::finalDistanceFile2()
        {
            Scalar dist, area, asymm, distfunc;
            Scalar step = m_threshold_max / NBMAX_STEP;

            Scalar areas[NBMAX_STEP] = {0};

            // computing the median distance value
            DistanceSorting::iterator it = m_distSort.begin();

            if (m_distSort.size() % 2)
            {
                std::advance(it, m_distSort.size() / 2);
                m_distance_median = (*it).r;
            }
            else
            {
                std::advance(it, m_distSort.size() / 2 - 1);
                Scalar m = (*it).r;
                std::advance(it, 1);
                m_distance_median = (m + (*it).r) / 2;
            }

            LOG(logINFO) << "Distance median : " << m_distance_median;

            for (uint i = 0; i < m_distrib.size(); i++)
            {
                dist = m_distrib[i].r;
                if (dist > m_distance_median / 4)
                {
                    distfunc = 0;
                }
                else
                {
                    distfunc = std::pow(1 - std::pow((dist / (m_distance_median / 4)),2),2); // weight function for distance
                }

                area = m_facesArea[m_distrib[i].objId][m_distrib[i].faceId];
                asymm = m_facesAsymmetry[m_distrib[i].objId][m_distrib[i].faceId];
                if (asymm > m_asymmetry_median)
                {
                    asymm = 0;
                }
                else
                {
                    //asymm = 1 - (asymm / m_asymmetry_median);
                    asymm = std::pow(1 - std::pow((asymm / m_asymmetry_median),2),2); // weight function for anisotropy
                }

                int slot = std::floor(dist / step) - 1;
                areas[slot] = areas[slot] + area * asymm * distfunc;
            }

            std::ofstream file("Final_distrib2.txt", std::ios::out | std::ios::trunc);
            CORE_ASSERT(file, "Error while opening final distance distribution file.");

            for (uint i = 0; i < NBMAX_STEP; i++)
            {
                file << ((2 * i + 1) * step) / 2 << " " << areas[i] << std::endl;

//                if (areas[i] != 0)
//                {
                    std::pair<Scalar,Scalar> p;
                    p.first = ((2 * i + 1) * step) / 2;
                    p.second = areas[i];
                    m_finalDistrib2.push_back(p);
//                }
            }

            file.close();
        }

        void MeshContactManager::finalDistribCleaned()
        {
            m_finalDistribCleaned.clear();
            m_minSort.clear();
            m_maxSort.clear();

            // the end of the last cluster will be the first distance value after the last non zero value for area * f(asymm)
            int j = m_finalDistrib.size() - 1;
            while (j >= 0 && m_finalDistrib[j].second == 0)
            {
                j--;
            }

            // finding minima and maxima of the final distribution
            uint i = 0;
            Scalar area = m_finalDistrib[i].second;
            Scalar area2 = m_finalDistrib[i+1].second;

            if (area <= area2)
            {
                m_finalDistribCleaned.push_back(m_finalDistrib[i]);
                //m_minSort.insert(m_finalDistrib[i]);
                while (i < j)
                {
                    do
                    {
                        i++;
                        area = m_finalDistrib[i].second;
                        area2 = m_finalDistrib[i+1].second;
                    } while (area <= area2 && i < j);
                    m_finalDistribCleaned.push_back(m_finalDistrib[i]);
//                    m_maxSort.insert(m_finalDistrib[i]);
                    if (! m_maxSort.insert(m_finalDistrib[i]).second)
                    {
                        LOG(logINFO) << "Insert of ( " << m_finalDistrib[i].first << " , " << m_finalDistrib[i].second << " ) in m_maxSort failed";
                    }
                    if (i < j)
                    {
                        do
                        {
                            i++;
                            area = m_finalDistrib[i].second;
                            area2 = m_finalDistrib[i+1].second;
                        } while (area >= area2 && i < j);
                        if (i < j)
                        {
                            m_finalDistribCleaned.push_back(m_finalDistrib[i]);
                            //                        m_minSort.insert(m_finalDistrib[i]);
                            if (! m_minSort.insert(m_finalDistrib[i]).second)
                            {
                                LOG(logINFO) << "Insert of ( " << m_finalDistrib[i].first << " , " << m_finalDistrib[i].second << " ) in m_minSort failed";
                            }
                        }
                    }
                }

                m_finalDistribCleaned.push_back(m_finalDistrib[j+1]);
            }

            else
            {
                m_finalDistribCleaned.push_back(m_finalDistrib[i]);
                //m_maxSort.insert(m_finalDistrib[i]);
                while (i < j)
                {
                    do
                    {
                        i++;
                        area = m_finalDistrib[i].second;
                        area2 = m_finalDistrib[i+1].second;
                    } while (area >= area2 && i < j);
                    if (i < j)
                    {
                        m_finalDistribCleaned.push_back(m_finalDistrib[i]);
    //                    m_minSort.insert(m_finalDistrib[i]);
                        if (! m_minSort.insert(m_finalDistrib[i]).second)
                        {
                            LOG(logINFO) << "Insert of ( " << m_finalDistrib[i].first << " , " << m_finalDistrib[i].second << " ) in m_minSort failed";
                        }

                        do
                        {
                            i++;
                            area = m_finalDistrib[i].second;
                            area2 = m_finalDistrib[i+1].second;
                        } while (area <= area2 && i < j);

                        m_finalDistribCleaned.push_back(m_finalDistrib[i]);
//                        m_maxSort.insert(m_finalDistrib[i]);
                        if (! m_maxSort.insert(m_finalDistrib[i]).second)
                        {
                            LOG(logINFO) << "Insert of ( " << m_finalDistrib[i].first << " , " << m_finalDistrib[i].second << " ) in m_maxSort failed";
                        }
                    }
                }

                m_finalDistribCleaned.push_back(m_finalDistrib[j+1]);
            }

            std::ofstream file("Final_distrib_cleaned.txt", std::ios::out | std::ios::trunc);
            CORE_ASSERT(file, "Error while opening cleaned final distance distribution file.");

            for (uint i = 0; i < m_finalDistribCleaned.size(); i++)
            {
                file << m_finalDistribCleaned[i].first << " " << m_finalDistribCleaned[i].second << std::endl;
            }

            file.close();
        }

        void MeshContactManager::topologicalPersistence()
        {
            m_diffSort.clear();
            m_plotSort.clear();
            m_finalDistrib3.clear();
            m_finalClusters3.clear();

            // finding (min,max) pairs and sorting them by descending difference
            MinSorting::iterator itMin = m_minSort.begin();
            MaxSorting::iterator itMax = m_maxSort.begin();

            std::pair<std::pair<Scalar,Scalar>, std::pair<Scalar,Scalar> > pairMinMax;

            std::ofstream file("Persistence_diagram.txt", std::ios::out | std::ios::trunc);
            CORE_ASSERT(file, "Error while opening persistence diagram file.");

            while (itMin != m_minSort.end())
            {
                while ((*std::next(itMax)).first < (*itMin).first && std::next(itMax) != m_maxSort.end())
                {
                    std::advance(itMax,1);
                }

                if (std::next(itMax) != m_maxSort.end())
                {
                    Scalar diff1 = (*itMax).second - (*itMin).second;
                    Scalar diff2 = (*std::next(itMax)).second - (*itMin).second;
                    if (diff2 < diff1 && diff2 >= 0)
                    {
                        std::advance(itMax,1);
                    }

                    pairMinMax.first = *itMin;
                    pairMinMax.second = *itMax;
                    m_diffSort.insert(pairMinMax);
                    if (pairMinMax.second.second < pairMinMax.first.second)
                    {
                        LOG(logINFO) << "Wrong insert in persistence diagram";
                        LOG(logINFO) << "Min : (" << (*itMin).first << "," << (*itMin).second <<")";
                        LOG(logINFO) << "Max : (" << (*itMax).first << "," << (*itMax).second <<")";

                        MaxSorting::iterator itMax2 = m_maxSort.begin();
                        LOG(logINFO) << "Max list : ";
                        while (itMax2 != m_maxSort.end())
                        {
                            LOG(logINFO) << "Max : (" << (*itMax2).first << "," << (*itMax2).second <<")";
                            std::advance(itMax2,1);
                        }
                    }
                    m_minSort.erase(itMin);
                    m_maxSort.erase(itMax);

                    file << pairMinMax.first.second << " " << pairMinMax.second.second << std::endl;

                    itMin = m_minSort.begin();
                    itMax = m_maxSort.begin();
                }
                else // last pair
                {
                    pairMinMax.first = *itMin;
                    pairMinMax.second = *itMax;
                    m_diffSort.insert(pairMinMax);
                    if (pairMinMax.second.second < pairMinMax.first.second)
                    {
                        LOG(logINFO) << "Wrong last insert in persistence diagram";
                        LOG(logINFO) << "Min : (" << (*itMin).first << "," << (*itMin).second <<")";
                        LOG(logINFO) << "Max : (" << (*itMax).first << "," << (*itMax).second <<")";

                        MaxSorting::iterator itMax2 = m_maxSort.begin();
                        LOG(logINFO) << "Max list : ";
                        while (itMax2 != m_maxSort.end())
                        {
                            LOG(logINFO) << "Max : (" << (*itMax2).first << "," << (*itMax2).second <<")";
                            std::advance(itMax2,1);
                        }
                    }
                    m_minSort.erase(itMin);
                    m_maxSort.erase(itMax);

                    file << pairMinMax.first.second << " " << pairMinMax.second.second << std::endl;

                    itMin = m_minSort.end();
                }
//                else
//                {
//                    if ((*itMax).second > (*itMin).second)
//                    {
//                        pairMinMax.first = *itMin;
//                        pairMinMax.second = *itMax;
//                        m_diffSort.insert(pairMinMax);
//                        m_minSort.erase(itMin);
//                        m_maxSort.erase(itMax);

//                        file << pairMinMax.first.second << " " << pairMinMax.second.second << std::endl;

//                        itMin = m_minSort.begin();
//                        itMax = m_maxSort.begin();
//                    }
//                }
            }

            file.close();

            // ploting using topological persistence
            m_plotSort.insert(m_finalDistribCleaned[0]); // first point
//            if (m_minSort.size() > 0)
//            {
//                while (itMin != m_minSort.end())
//                {
//                    m_plotSort.insert(*itMin);
//                    std::advance(itMin,1);
//                }
//            }
            if (m_maxSort.size() > 0)
            {
//                while (itMax != m_maxSort.end())
//                {
//                    m_plotSort.insert(*itMax);

//                    std::advance(itMax,1);
//                }
                m_plotSort.insert(*itMax); // inserting greatest max
            }

            m_plotSort.insert(m_finalDistribCleaned[m_finalDistribCleaned.size()-1]); // last point

            DiffSorting::iterator itDiff = m_diffSort.begin();

            for (uint i = 0; i < m_nbclusters_compute - 1; i++)
            {
                m_plotSort.insert((*itDiff).first);
                m_plotSort.insert((*itDiff).second);
                std::advance(itDiff,1);
            }

            std::ofstream file2("Topological_persistence.txt", std::ios::out | std::ios::trunc);
            CORE_ASSERT(file2, "Error while opening topological persistence file.");

            PlotSorting::iterator itPlot = m_plotSort.begin();
            while (itPlot != m_plotSort.end())
            {
                file2 << (*itPlot).first << " " << (*itPlot).second << std::endl;
                m_finalDistrib3.push_back(*itPlot);
                std::advance(itPlot,1);
            }

            file2.close();

            // finding minimums and maximums to clusterize
            uint k = 0;
            Scalar area = m_finalDistrib3[k].second;
            Scalar area2 = m_finalDistrib3[k+1].second;

            if (area <= area2)
            {
                while (k < m_finalDistrib3.size() - 1)
                {
                    do
                    {
                        k++;
                        area = m_finalDistrib3[k].second;
                        area2 = m_finalDistrib3[k+1].second;
                    } while (area <= area2 && k < m_finalDistrib3.size() - 1);
                    if (k < m_finalDistrib3.size())
                    {
                        do
                        {
                            k++;
                            area = m_finalDistrib3[k].second;
                            area2 = m_finalDistrib3[k+1].second;
                        } while (area >= area2 && k < m_finalDistrib3.size() - 1);
                        m_finalClusters3.push_back(m_finalDistrib3[k].first);
                    }
                }
            }

            else
            {
                while (k < m_finalDistrib3.size() - 1)
                {
                    do
                    {
                        k++;
                        area = m_finalDistrib3[k].second;
                        area2 = m_finalDistrib3[k+1].second;
                    } while (area >= area2 && k < m_finalDistrib3.size() - 1);
                    m_finalClusters3.push_back(m_finalDistrib3[k].first);
                    if (k < m_finalDistrib3.size() - 1)
                    {
                        do
                        {
                            k++;
                            area = m_finalDistrib3[k].second;
                            area2 = m_finalDistrib3[k+1].second;
                        } while (area <= area2 && k < m_finalDistrib3.size() - 1);
                    }
                }
            }

            for (uint l = 0; l < m_finalClusters3.size(); l++)
            {
                LOG(logINFO) << "Cluster " << l + 1 << " : " << m_finalClusters3[l];
            }
        }

        void MeshContactManager::clusterFiltering()
        {
            Scalar dist = m_finalClusters3[0];

            std::vector<std::pair<int,int> > pairsToRemove;

            DistanceSorting::iterator it;
            bool proximity;

            for (uint i = 0; i < m_meshContactElements.size(); i++)
            {
                for (uint j = i + 1; j < m_meshContactElements.size(); j++)
                {
                    if (m_proximityPairs(i,j))
                    {
                        it = m_distSort.begin();
                        proximity = false;
                        while ((*it).r <= dist && !proximity && it != m_distSort.end())
                        {
                            if (((*it).objId == i && (*it).otherObjId == j) || ((*it).objId == j && (*it).otherObjId == i))
                            {
                                proximity = true;
                            }
                            else
                            {
                                ++it;
                            }
                        }
                        if (!proximity)
                        {
                            std::pair<int, int> p;
                            p.first = i;
                            p.second = j;
                            pairsToRemove.push_back(p);
                        }
                    }
                }
            }

            for (auto& pair : pairsToRemove)
            {
                int i = 0;

                while (i < m_distrib.size())
                {
                    if ((m_distrib[i].objId == pair.first && m_distrib[i].otherObjId == pair.second) || (m_distrib[i].objId == pair.second && m_distrib[i].otherObjId == pair.first))
                    {
                        m_distrib.erase(m_distrib.begin() + i);
                    }
                    else
                    {
                        i++;
                    }
                }
            }

            if (pairsToRemove.size() > 0)
            {
                m_distSort.clear();
                m_asymmSort.clear();
                m_threshold_max = 0;
                m_asymmetry_max = 0;
                m_finalDistrib.clear();

                for (auto& pt : m_distrib)
                {
                    m_distSort.insert(pt);
                    m_asymmSort.insert(pt);
                    if (pt.r > m_threshold_max)
                    {
                        m_threshold_max = pt.r;
                    }
                    if (pt.a > m_asymmetry_max)
                    {
                        m_asymmetry_max = pt.a;
                    }
                }

                finalDistanceFile();
                finalDistribCleaned();
                if (m_nbclusters_compute > m_finalDistribCleaned.size() / 2 + 1)
                {
                    m_nbclusters_compute = m_finalDistribCleaned.size() / 2 + 1;
                }
                topologicalPersistence();
            }
        }

//        int MeshContactManager::nbClusters()
//        {
//            int i = 0;

//            PlotSorting::iterator plotIt = m_plotSort.begin();
//            Scalar area = (*plotIt).second;
//            std::advance(plotIt,1);
//            Scalar area2 = (*plotIt).second;

//            if (area <= area2)
//            {
//                while (plotIt != m_plotSort.end())
//                {
//                    do
//                    {
//                        area = (*plotIt).second;
//                        std::advance(plotIt,1);
//                        area2 = (*plotIt).second;
//                    } while (area <= area2 && plotIt != m_plotSort.end());
//                    if (plotIt != m_plotSort.end())
//                    {
//                        do
//                        {
//                            area = (*plotIt).second;
//                            std::advance(plotIt,1);
//                            area2 = (*plotIt).second;
//                        } while (area >= area2 && plotIt != m_plotSort.end());
//                        i++;
//                    }
//                    else
//                    {
//                        i++;
//                    }
//                }
//            }

//            else
//            {
//                do
//                {
//                    area = (*plotIt).second;
//                    std::advance(plotIt,1);
//                    area2 = (*plotIt).second;
//                } while (area >= area2 && plotIt != m_plotSort.end());
//                i++;

//                while (plotIt != m_plotSort.end())
//                {
//                    do
//                    {
//                        area = (*plotIt).second;
//                        std::advance(plotIt,1);
//                        area2 = (*plotIt).second;
//                    } while (area <= area2 && plotIt != m_plotSort.end());
//                    if (plotIt != m_plotSort.end())
//                    {
//                        do
//                        {
//                            area = (*plotIt).second;
//                            std::advance(plotIt,1);
//                            area2 = (*plotIt).second;
//                        } while (area >= area2 && plotIt != m_plotSort.end());
//                        i++;
//                    }
//                    else
//                    {
//                        i++;
//                    }
//                }
//            }

//            return i;
//        }

        void MeshContactManager::findClusters()
        {
            uint i = 0;
            Scalar area = m_finalDistrib[i].second;
            Scalar area2 = m_finalDistrib[i+1].second;

            // defining first cluster
            if (area > area2)
            {
                // lower side of gaussian
                do
                {
                    i++;
                    area = m_finalDistrib[i].second;
                    area2 = m_finalDistrib[i+1].second;
                } while (area >= area2 && i < m_finalDistrib.size());
                if (i == m_finalDistrib.size())
                {
                    m_finalClusters.push_back(m_finalDistrib[i - 1].first); // end of last cluster
                }
                else
                {
                    m_finalClusters.push_back(m_finalDistrib[i].first); // end of first cluster
                }
            }
            else
            {
                // upper side of gaussian
                do
                {
                    i++;
                    area = m_finalDistrib[i].second;
                    area2 = m_finalDistrib[i+1].second;
                } while (area <= area2 && i < m_finalDistrib.size());
                if (i == m_finalDistrib.size())
                {
                    m_finalClusters.push_back(m_finalDistrib[i - 1].first); // end of last cluster
                }
                else
                {
                    // lower side of gaussian
                    do
                    {
                        i++;
                        area = m_finalDistrib[i].second;
                        area2 = m_finalDistrib[i+1].second;
                    } while (area >= area2 && i < m_finalDistrib.size());
                    if (i == m_finalDistrib.size())
                    {
                        m_finalClusters.push_back(m_finalDistrib[i - 1].first); // end of last cluster
                    }
                    else
                    {
                        m_finalClusters.push_back(m_finalDistrib[i].first); // end of first cluster
                    }
                }
            }

            // defining following clusters
            while (i < m_finalDistrib.size() - 1)
            {
                // upper side of gaussian
                do
                {
                    i++;
                    area = m_finalDistrib[i].second;
                    area2 = m_finalDistrib[i+1].second;
                } while (area <= area2 && i < m_finalDistrib.size());
                if (i == m_finalDistrib.size())
                {
                    m_finalClusters.push_back(m_finalDistrib[i - 1].first); // end of last cluster
                }
                else
                {
                    // lower side of gaussian
                    do
                    {
                        i++;
                        area = m_finalDistrib[i].second;
                        area2 = m_finalDistrib[i+1].second;
                    } while (area >= area2 && i < m_finalDistrib.size());
                    if (i == m_finalDistrib.size())
                    {
                        m_finalClusters.push_back(m_finalDistrib[i - 1].first); // end of last cluster
                    }
                    else
                    {
                        m_finalClusters.push_back(m_finalDistrib[i].first); // end of cluster
                    }
                }
            }

            // the end of the last cluster will be the first distance value after the last non zero value for area * f(asymm)
            uint j = m_finalDistrib.size() - 1;
            while (j >= 0 && m_finalDistrib[j].second == 0)
            {
                j--;
            }
            m_finalClusters[m_finalClusters.size() - 1] = m_finalDistrib[j + 1].first;

            LOG(logINFO) << "Number of clusters : " << m_finalClusters.size();

            for (uint j = 0; j < m_finalClusters.size(); j++)
            {
                LOG(logINFO) << "End of cluster " << j + 1 << ": " << m_finalClusters[j];
            }
        }

        void MeshContactManager::findClusters2()
        {
            uint i = 0;
            Scalar area = m_finalDistrib2[i].second;
            Scalar area2 = m_finalDistrib2[i+1].second;

            // defining first cluster
            if (area > area2)
            {
                // lower side of gaussian
                do
                {
                    i++;
                    area = m_finalDistrib2[i].second;
                    area2 = m_finalDistrib2[i+1].second;
                } while (area >= area2 && i < m_finalDistrib2.size());
                if (i == m_finalDistrib2.size())
                {
                    m_finalClusters2.push_back(m_finalDistrib2[i - 1].first); // end of last cluster
                }
                else
                {
                    m_finalClusters2.push_back(m_finalDistrib2[i].first); // end of first cluster
                }
            }
            else
            {
                // upper side of gaussian
                do
                {
                    i++;
                    area = m_finalDistrib2[i].second;
                    area2 = m_finalDistrib2[i+1].second;
                } while (area <= area2 && i < m_finalDistrib2.size());
                if (i == m_finalDistrib2.size())
                {
                    m_finalClusters2.push_back(m_finalDistrib2[i - 1].first); // end of last cluster
                }
                else
                {
                    // lower side of gaussian
                    do
                    {
                        i++;
                        area = m_finalDistrib2[i].second;
                        area2 = m_finalDistrib2[i+1].second;
                    } while (area >= area2 && i < m_finalDistrib2.size());
                    if (i == m_finalDistrib2.size())
                    {
                        m_finalClusters2.push_back(m_finalDistrib2[i - 1].first); // end of last cluster
                    }
                    else
                    {
                        m_finalClusters2.push_back(m_finalDistrib2[i].first); // end of first cluster
                    }
                }
            }

            // defining following clusters
            while (i < m_finalDistrib2.size() - 1)
            {
                // upper side of gaussian
                do
                {
                    i++;
                    area = m_finalDistrib2[i].second;
                    area2 = m_finalDistrib2[i+1].second;
                } while (area <= area2 && i < m_finalDistrib2.size());
                if (i == m_finalDistrib2.size())
                {
                    m_finalClusters2.push_back(m_finalDistrib2[i - 1].first); // end of last cluster
                }
                else
                {
                    // lower side of gaussian
                    do
                    {
                        i++;
                        area = m_finalDistrib2[i].second;
                        area2 = m_finalDistrib2[i+1].second;
                    } while (area >= area2 && i < m_finalDistrib2.size());
                    if (i == m_finalDistrib2.size())
                    {
                        m_finalClusters2.push_back(m_finalDistrib2[i - 1].first); // end of last cluster
                    }
                    else
                    {
                        m_finalClusters2.push_back(m_finalDistrib2[i].first); // end of cluster
                    }
                }
            }

            // the end of the last cluster will be the first distance value after the last non zero value for area * f(asymm)
            uint j = m_finalDistrib2.size() - 1;
            while (j >= 0 && m_finalDistrib2[j].second == 0)
            {
                j--;
            }
            m_finalClusters2[m_finalClusters2.size() - 1] = m_finalDistrib2[j + 1].first;

            LOG(logINFO) << "Number of clusters : " << m_finalClusters2.size();

            for (uint j = 0; j < m_finalClusters2.size(); j++)
            {
                LOG(logINFO) << "End of cluster " << j + 1 << ": " << m_finalClusters2[j];
            }
        }

        void MeshContactManager::thresholdComputation()
        {
            m_broader_threshold = m_threshold / (std::pow(1 - std::pow(m_influence,1/m_n),1/m_m));
            //m_broader_threshold = m_threshold;
        }

        void MeshContactManager::setComputeAlpha()
        {
            constructPriorityQueues2();
            //constructPriorityQueues1();

            Ra::Core::PriorityQueue::PriorityQueueContainer::iterator it;

            // finding maximum error value
            Scalar errorMax = 0;
            for (uint i = 0; i < m_nbobjects; i++)
            {
                MeshContactElement* obj = m_meshContactElements[i];
                it = obj->getPriorityQueue()->getPriorityQueueContainer().begin();
                std::advance(it,obj->getPriorityQueue()->getPriorityQueueContainer().size() - 1);
                if ((*it).m_err > errorMax)
                {
                    errorMax = (*it).m_err;
                }
            }
            LOG(logINFO) << "Error max : " << errorMax;

            // computing error distribution, error mean and median values
            Scalar step = errorMax / NBMAX_STEP_ERROR;
            int errorArray[NBMAX_STEP_ERROR] = {0};
            //Scalar errorArray[NBMAX_STEP_ERROR] = {0};

            Scalar errorMean = 0;
            Scalar errorMedian;
            Scalar errorQuartile;
            int nbEdges = 0;
            for (uint i = 0; i < m_nbobjects; i++)
            {
                nbEdges += m_meshContactElements[i]->getPriorityQueue()->getPriorityQueueContainer().size();
            }

            struct compareErrorByAscendingValue
            {
                inline bool operator() (const Scalar &e1, const Scalar &e2) const
                {
                    return e1 <= e2;
                }
            };
            typedef std::set<Scalar, compareErrorByAscendingValue> ErrorSorting;
//            struct compareErrorByAscendingValue
//            {
//                inline bool operator() (const std::pair<Scalar,Scalar> &e1, const std::pair<Scalar,Scalar> &e2) const
//                {
//                    return e1.first <= e2.first;
//                }
//            };
//            typedef std::set<std::pair<Scalar,Scalar>, compareErrorByAscendingValue> ErrorSorting;
            ErrorSorting errorSort;

            for (uint i = 0; i < m_nbobjects; i++)
            {
                MeshContactElement* obj = m_meshContactElements[i];
                it = obj->getPriorityQueue()->getPriorityQueueContainer().begin();
                int j = 0;
                while (j < NBMAX_STEP_ERROR && it != obj->getPriorityQueue()->getPriorityQueueContainer().end())
                {
                    while ((*it).m_err < step * (j + 1) && it != obj->getPriorityQueue()->getPriorityQueueContainer().end())
                    {
                        Scalar err = (*it).m_err;
                        errorMean += err;
                        errorArray[j]++;
//                        std::pair<Scalar,Scalar> err;
//                        err.first = (*it).m_err;
//                        err.second = std::sqrt((obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_vertex[(*it).m_vs_id]->P() - obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_vertex[(*it).m_vt_id]->P()).squaredNorm());
//                        errorMean += err.first;
//                        errorArray[j] += err.second;
                        if (!errorSort.insert(err).second)
                        {
                            LOG(logINFO) << "Error insert failed";
                        }
                        ++it;
                    }
                    j++;
                }
            }

            errorMean /= nbEdges;
            LOG(logINFO) << "Mean error : " << errorMean;

            ErrorSorting::iterator medianIt = errorSort.begin();
            if (nbEdges % 2 == 0)
            {
                std::advance(medianIt, nbEdges/2);
                errorMedian = (*medianIt);
                //errorMedian = (*medianIt).first;
                ++medianIt;
                errorMedian += (*medianIt);
                //errorMedian += (*medianIt).first;
                errorMedian /= 2;
            }
            else
            {
                std::advance(medianIt, nbEdges/2 + 1);
                errorMedian = (*medianIt);
                //errorMedian = (*medianIt).first;
            }
            LOG(logINFO) << "Median error : " << errorMedian;

            medianIt = errorSort.begin();
            if (nbEdges % 4 == 0 || nbEdges % 4 == 1)
            {
                std::advance(medianIt, nbEdges/4 - 1);
                errorQuartile = (*medianIt);
                //errorQuartile = (*medianIt).first;
                ++medianIt;
                errorQuartile += (*medianIt);
                //errorQuartile += (*medianIt).first;
                errorQuartile /= 2;
            }
            else
            {
                std::advance(medianIt, nbEdges/4);
                errorQuartile = (*medianIt);
                //errorQuartile = (*medianIt).first;
            }

            if (errorQuartile == 0)
            {
                ErrorSorting::iterator zeroIt = errorSort.begin();
                int nbZeros = 0;
                if ((*zeroIt) == 0)
//                if ((*zeroIt).first == 0)
                {
                    while ((*zeroIt) == 0)
//                    while ((*zeroIt).first == 0)
                    {
                        nbZeros++;
                        ++zeroIt;
                    }
                }

                medianIt = errorSort.begin();
                if ((nbEdges - nbZeros) % 4 == 0 || (nbEdges - nbZeros) % 4 == 1)
                {
                    std::advance(medianIt, nbZeros + (nbEdges - nbZeros)/4 - 1);
                    errorQuartile = (*medianIt);
                    //errorQuartile = (*medianIt).first;
                    ++medianIt;
                    errorQuartile += (*medianIt);
                    //errorQuartile += (*medianIt).first;
                    errorQuartile /= 2;
                }
                else
                {
                    std::advance(medianIt, nbZeros + (nbEdges - nbZeros)/4);
                    errorQuartile = (*medianIt);
                    //errorQuartile = (*medianIt).first;
                }
            }

            LOG(logINFO) << "Quartile error : " << errorQuartile;

            std::ofstream file("Error_distrib.txt", std::ios::out | std::ios::trunc);
            CORE_ASSERT(file, "Error while opening error distribution file.");
            for (uint i = 0; i < NBMAX_STEP_ERROR; i++)
            {
               file << step * (i + 1) << " " << errorArray[i] << std::endl;
            }
            file.close();

            Scalar errorFirstCluster;
            //Scalar errorMeanFirstCluster;
            int k = 0;
            int nbError = errorArray[k];
            int nbErrorNext = errorArray[k+1];
//            Scalar nbError = errorArray[k];
//            Scalar nbErrorNext = errorArray[k+1];
            if (nbError <= nbErrorNext)
            {
                do
                {
                    nbError = errorArray[k];
                    nbErrorNext = errorArray[k+1];
                } while (nbError <= nbErrorNext && k < NBMAX_STEP_ERROR - 1);
                if (k < NBMAX_STEP_ERROR - 1)
                {
                    do
                    {
                        k++;
                        nbError = errorArray[k];
                        nbErrorNext = errorArray[k+1];
                    } while (nbError >= nbErrorNext && k < NBMAX_STEP_ERROR - 1);
                }
            }
            else
            {
                do
                {
                    k++;
                    nbError = errorArray[k];
                    nbErrorNext = errorArray[k+1];
                } while (nbError >= nbErrorNext && k < NBMAX_STEP_ERROR - 1);
            }
            errorFirstCluster = step * (k + 1);
            //errorMeanFirstCluster = errorFirstCluster / 2;
            LOG(logINFO) << "Error first cluster : " << errorFirstCluster;

            //while (errorQuartile == 0)
            //{
        // std::advance(medianIt, 1);
        // errorQuartile = (*medianIt);
        // }

            m_lambda = (errorFirstCluster / errorQuartile - 1) / std::pow(m_broader_threshold, 2);
            //m_lambda = (errorFirstCluster / errorMean - 1) / std::pow(m_broader_threshold, 2);
            //m_lambda = (errorFirstCluster / errorMedian - 1) / std::pow(m_broader_threshold, 2);
            //m_lambda = (errorFirstCluster / errorMeanFirstCluster - 1) / std::pow(m_broader_threshold, 2);

            LOG(logINFO) << "Alpha : " << m_lambda;

            int nbFacesInit = 0;
            for (auto& obj : m_meshContactElements)
            {
                nbFacesInit += obj->getInitTriangleMesh().m_triangles.size();
            }

//            int nbProximityPairs = 0;
//            for (uint i = 0; i < m_meshContactElements.size(); i++)
//            {
//                for (uint j = i + 1; j < m_meshContactElements.size(); j++)
//                {
//                    if (m_proximityPairs(i,j))
//                    {
//                        nbProximityPairs++;
//                    }
//                }
//            }

            Scalar proximity_percentage = (m_broader_threshold / m_aabb_scene.diagonal()) * 100;

            std::ofstream file2("Parameters.txt", std::ios::out | std::ios::trunc);
            CORE_ASSERT(file2, "Error while opening parameters file.");
            file2 << "Nb objects scene : " << m_meshContactElements.size() << std::endl;
            file2 << "Nb faces init : " << nbFacesInit << std::endl;
            //file2 << "Nb proximity object pairs : " << nbProximityPairs << std::endl;
            //file2 << "Nb clusters computed : " << m_nbclusters_compute << std::endl;
            //file2 << "Nb clusters selected : " << m_nbclusters_display << std::endl;
            //file2 << "Cluster threshold : " << m_finalClusters3[m_nbclusters_display - 1] << std::endl;
            file2 << "Weight function parameter m : " << m_m << std::endl;
            file2 << "Weight function parameter n : " << m_n << std::endl;
            file2 << "Cluster threshold weight : " << m_influence << std::endl;
            file2 << "Proximity threshold : " << m_broader_threshold << std::endl;
            file2 << "Scene AABB diag : " << m_aabb_scene.diagonal() << std::endl;
            file2 << "Proximity threshold percentage of scene AABB diag : " << proximity_percentage << std::endl;
            file2 << "Nb objects to simplify : " << m_nbobjects << std::endl;
            file2 << "Quartile error : " << errorQuartile << std::endl;
            file2 << "Error first cluster : " << errorFirstCluster << std::endl;
            file2 << "Proximity weight parameter alpha: " << m_lambda << std::endl;
            file2.close();
        }

        int MeshContactManager::getAlpha()
        {
            return (int)m_lambda;
        }

        void MeshContactManager::setLodValueChanged(int value)
        {
            if (m_nbfaces < value)
            {
                while (m_nbfaces < value)
                {
                    MeshContactElement* obj = static_cast<MeshContactElement*>(m_meshContactElements[m_index_pmdata[--m_curr_vsplit]]);
                    int nbfaces = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getNbFaces();
                    if (! obj->getProgressiveMeshLOD()->more())
                        break;
                    else
                    {
                        m_nbfaces += (obj->getProgressiveMeshLOD()->getProgressiveMesh()->getNbFaces() - nbfaces);
                    }
                }
            }
            else if (m_nbfaces > value)
            {
                while (m_nbfaces > value)
                {
                    MeshContactElement* obj = static_cast<MeshContactElement*>(m_meshContactElements[m_index_pmdata[m_curr_vsplit++]]);
                    int nbfaces = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getNbFaces();
                    if (! obj->getProgressiveMeshLOD()->less())
                        break;
                    else
                    {
                        m_nbfaces -= (nbfaces - obj->getProgressiveMeshLOD()->getProgressiveMesh()->getNbFaces());
                    }
                }
            }

            for (const auto& elem : m_meshContactElements)
            {
                MeshContactElement* obj = static_cast<MeshContactElement*>(elem);
                Ra::Core::TriangleMesh newMesh;
                Ra::Core::convertPM(*(obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()), newMesh);
                obj->updateTriangleMesh(newMesh);
            }
        }

        void MeshContactManager::setThresholdValueChanged(int value)
        {
            Scalar th = (Scalar)(value) / PRECISION;
            displayDistribution(th, m_asymmetry);
        }

        void MeshContactManager::setAsymmetryValueChanged(int value)
        {
            Scalar as = (Scalar)(value) / PRECISION;
            displayDistribution(m_threshold, as);
        }

        void MeshContactManager::setComputeR()
        {
            computeAABB();
            proximityPairs();

            distanceAsymmetryDistribution();
            LOG(logINFO) << "Distance asymmetry distributions computed.";
        }

        void MeshContactManager::setComputeDistribution()
        {
            distanceAsymmetryFile2();
            computeFacesArea();
            weightedDistanceFile();
            computeFacesAsymmetry();
            finalDistanceFile();
        }

        void MeshContactManager::setLoadDistribution(std::string filePath)
        {
            computeAABB();
            proximityPairs();

            loadDistribution(filePath);
            LOG(logINFO) << "Distance asymmetry distributions loaded.";
        }


        // Display proximity zones
        void MeshContactManager::setDisplayProximities()
        {
            if (m_lambda != 0.0)
            {
                thresholdComputation(); // computing m_broader_threshold
            }

            constructPriorityQueues2();
            //constructPriorityQueues1();
        }

        void MeshContactManager::kmeans(int k)
        {
            // selecting k random points to be the clusters centers
            int nbPtDistrib = m_distrib.size();
            std::vector<int> indices;
            for (uint i = 0; i < nbPtDistrib; i++)
            {
                indices.push_back(i);
            }
            std::srand(std::time(0));
            std::random_shuffle(indices.begin(), indices.end());
            for (uint i = 0; i < k; i++)
            {
                std::vector<int> id;
                std::pair<Scalar, std::vector<int> > cluster;
                cluster.first = m_distrib[indices[i]].r;
                cluster.second = id;
                m_clusters.push_back(cluster);
            }

            bool stableMeans;

            do
            {
                // clustering all the faces
                for (uint i = 0; i < k; i++)
                {
                    m_clusters[i].second.clear();
                }
                for (uint i = 0; i < nbPtDistrib; i++)
                {
                    int closestCenterId = 0;
                    Scalar closestDist = abs(m_distrib[i].r - m_clusters[0].first);
                    for (uint j = 1; j <k; j++)
                    {
                        Scalar dist = abs(m_distrib[i].r - m_clusters[j].first);
                        if (dist < closestDist)
                        {
                            closestDist = dist;
                            closestCenterId = j;
                        }
                    }
                    m_clusters[closestCenterId].second.push_back(i);
                }

//                for (uint i = 0; i < k; i++)
//                {
//                    LOG(logINFO) << "Center and number of faces of cluster " << i + 1 << " : " << m_clusters[i].first << " " << m_clusters[i].second.size();
//                }

                // updating cluster centers
                stableMeans = true;
                for (uint i = 0; i < k; i++)
                {
                    Scalar meanCluster = 0;
                    int nbPtDistribCluster = m_clusters[i].second.size();
                    for (uint j = 0; j < nbPtDistribCluster; j++)
                    {
                        meanCluster += m_distrib[m_clusters[i].second[j]].r;
                    }
                    meanCluster /= nbPtDistribCluster;
                    if (m_clusters[i].first != meanCluster)
                    {
                        stableMeans = false;
                    }
                    m_clusters[i].first = meanCluster;
                }
            } while (!stableMeans);
        }

        Scalar MeshContactManager::silhouette()
        {
            Scalar S = 0;
            int nbPtDistrib = m_distrib.size();
            for (uint i = 0; i < m_clusters.size(); i++)
            {
                int nbPtDistribCluster = m_clusters[i].second.size();
                for (uint j = 0; j < nbPtDistribCluster; j++)
                {
                    Scalar a = 0;
                    for (uint k = 0; k < nbPtDistribCluster; k++)
                    {
                        if (k != j)
                        {
                            a += abs(m_distrib[m_clusters[i].second[j]].r - m_distrib[m_clusters[i].second[k]].r);
                        }
                    }
                    a /= (nbPtDistribCluster - 1);

                    Scalar b;
                    int start;
                    if (i == 0)
                    {
                        start = 1;
                    }
                    else
                    {
                        start = 0;
                    }
                    b = abs(m_distrib[m_clusters[i].second[j]].r - m_clusters[start].first);
                    for (uint k = start; k < m_clusters.size(); k++)
                    {
                        if (k != i)
                        {
                            Scalar b_cluster = abs(m_distrib[m_clusters[i].second[j]].r - m_clusters[k].first);
                            if (b_cluster < b)
                            {
                                b = b_cluster;
                            }
                        }
                    }

                    Scalar s = (b - a) / std::max(a,b);
                    S += s;
                }
            }
            S /= nbPtDistrib;
            return S;
        }


        void MeshContactManager::clustering(Scalar silhouetteMin, int nbClustersMax)
        {
            int k = 1;
            Scalar S;

            Scalar maxS = -1;

            int bestNbClusters;
            std::vector<std::pair<Scalar, std::vector<int> > > bestClusters;

            do
            {
                k++;
                kmeans(k);
                S = silhouette();

                LOG(logINFO) << "Number of clusters : " << k << " and silhouette value : " << S;

                if (S > maxS)
                {
                    maxS = S;
                    bestNbClusters = k;
                    bestClusters = m_clusters;
                }
            } while (maxS < silhouetteMin && k < nbClustersMax);

            m_clusters = bestClusters;

            LOG(logINFO) << "Best number of clusters : " << bestNbClusters << " and silhouette value : " << maxS;
        }

//        void MeshContactManager::colorClusters()
//        {
//            int nbClusters = m_clusters.size();

//            // ordering clusters by distance in order to color them
//            struct compareCenterClusterByDistance
//            {
//                inline bool operator() (const std::pair<int,Scalar> &c1, const std::pair<int,Scalar> &c2) const
//                {
//                    return c1.second <= c2.second;
//                }
//            };

//            typedef std::set<std::pair<int,Scalar>, compareCenterClusterByDistance> ClusterSorting;

//            ClusterSorting clusters;

//            for (uint i = 0; i < nbClusters; i++)
//            {
//                std::pair<int,Scalar> cluster;
//                cluster.first = i;
//                cluster.second = m_clusters[i].first;
//                clusters.insert(cluster);
//            }

//            Ra::Core::Vector4 vertexColor (0, 0, 0, 0);
//            for (uint i = 0; i < m_meshContactElements.size(); i++)
//            {
//                MeshContactElement* obj = m_meshContactElements[i];
//                int nbVertices = obj->getMesh()->getGeometry().m_vertices.size();
//                Ra::Core::Vector4Array colors;
//                for (uint v = 0; v < nbVertices; v++)
//                {
//                    colors.push_back(vertexColor);
//                }
//                obj->getMesh()->addData(Ra::Engine::Mesh::VERTEX_COLOR, colors);
//            }

//            Ra::Core::Vector4 clusterColor (0, 0, 1, 0);
//            ClusterSorting::iterator it = clusters.begin();
//            int k = 0;
//            while (it != clusters.end())
//            {
//                int clusterId = (*it).first;
//                Scalar coeffCluster = (nbClusters - k) / nbClusters;

//                int nbFacesCluster = m_clusters[clusterId].second.size();
//                for (uint i = 0; i < nbFacesCluster; i++)
//                {
//                    MeshContactElement* obj = m_meshContactElements[m_distrib[m_clusters[clusterId].second[i]].objId];
//                    Ra::Core::VectorArray<Ra::Core::Triangle> t = obj->getTriangleMeshDuplicate().m_triangles;
//                    Ra::Core::Vector4Array colors = obj->getMesh()->getData(Ra::Engine::Mesh::VERTEX_COLOR);

//                    colors[t[m_distrib[m_clusters[clusterId].second[i]].faceId][0]] = coeffCluster * clusterColor;
//                    colors[t[m_distrib[m_clusters[clusterId].second[i]].faceId][1]] = coeffCluster * clusterColor;
//                    colors[t[m_distrib[m_clusters[clusterId].second[i]].faceId][2]] = coeffCluster * clusterColor;
//                    obj->getMesh()->addData(Ra::Engine::Mesh::VERTEX_COLOR, colors);
//                }

//                std::advance(it, 1);
//                k++;
//            }
//        }

        void MeshContactManager::colorClusters()
        {
            Ra::Core::Vector4 vertexColor (0, 0, 0, 0);
            for (uint i = 0; i < m_meshContactElements.size(); i++)
            {
                MeshContactElement* obj = m_meshContactElements[i];
                int nbVertices = obj->getMesh()->getGeometry().m_vertices.size();
                Ra::Core::Vector4Array colors;
                for (uint v = 0; v < nbVertices; v++)
                {
                    colors.push_back(vertexColor);
                }
                obj->getMesh()->addData(Ra::Engine::Mesh::VERTEX_COLOR, colors);
            }

            std::srand(std::time(0));
            DistanceSorting::iterator it = m_distSort.begin();
            for (uint i = 0; i < m_finalClusters.size(); i++)
            {
                Ra::Core::Vector4 clusterColor ((Scalar)(rand() % 1000) / (Scalar)1000, (Scalar)(rand() % 1000) / (Scalar)1000, (Scalar)(rand() % 1000) / (Scalar)1000, (Scalar)(rand() % 1000) / (Scalar)1000);
                while ((*it).r <= m_finalClusters[i] && it != m_distSort.end())
                {
                    MeshContactElement* obj = m_meshContactElements[(*it).objId];
                    Ra::Core::VectorArray<Ra::Core::Triangle> t = obj->getTriangleMeshDuplicate().m_triangles;
                    Ra::Core::Vector4Array colors = obj->getMesh()->getData(Ra::Engine::Mesh::VERTEX_COLOR);

                    colors[t[(*it).faceId][0]] = clusterColor;
                    colors[t[(*it).faceId][1]] = clusterColor;
                    colors[t[(*it).faceId][2]] = clusterColor;
                    obj->getMesh()->addData(Ra::Engine::Mesh::VERTEX_COLOR, colors);

                    std::advance(it, 1);
                }
            }
        }

        void MeshContactManager::colorClusters2()
        {
            Ra::Core::Vector4 vertexColor (0, 0, 0, 0);
            for (uint i = 0; i < m_meshContactElements.size(); i++)
            {
                MeshContactElement* obj = m_meshContactElements[i];
                int nbVertices = obj->getMesh()->getGeometry().m_vertices.size();
                Ra::Core::Vector4Array colors;
                for (uint v = 0; v < nbVertices; v++)
                {
                    colors.push_back(vertexColor);
                }
                obj->getMesh()->addData(Ra::Engine::Mesh::VERTEX_COLOR, colors);
            }

            std::srand(std::time(0));
            DistanceSorting::iterator it = m_distSort.begin();
            for (uint i = 0; i < m_finalClusters2.size(); i++)
            {
                Ra::Core::Vector4 clusterColor ((Scalar)(rand() % 1000) / (Scalar)1000, (Scalar)(rand() % 1000) / (Scalar)1000, (Scalar)(rand() % 1000) / (Scalar)1000, (Scalar)(rand() % 1000) / (Scalar)1000);
                while ((*it).r <= m_finalClusters2[i] && it != m_distSort.end())
                {
                    MeshContactElement* obj = m_meshContactElements[(*it).objId];
                    Ra::Core::VectorArray<Ra::Core::Triangle> t = obj->getTriangleMeshDuplicate().m_triangles;
                    Ra::Core::Vector4Array colors = obj->getMesh()->getData(Ra::Engine::Mesh::VERTEX_COLOR);

                    colors[t[(*it).faceId][0]] = clusterColor;
                    colors[t[(*it).faceId][1]] = clusterColor;
                    colors[t[(*it).faceId][2]] = clusterColor;
                    obj->getMesh()->addData(Ra::Engine::Mesh::VERTEX_COLOR, colors);

                    std::advance(it, 1);
                }
            }
        }

        void MeshContactManager::colorClusters3()
        {
            Ra::Core::Vector4 vertexColor (0, 0, 0, 0);
            for (uint i = 0; i < m_meshContactElements.size(); i++)
            {
                MeshContactElement* obj = m_meshContactElements[i];
                int nbVertices = obj->getMesh()->getGeometry().m_vertices.size();
                Ra::Core::Vector4Array colors;
                for (uint v = 0; v < nbVertices; v++)
                {
                    colors.push_back(vertexColor);
                }
                obj->getMesh()->addData(Ra::Engine::Mesh::VERTEX_COLOR, colors);
            }

            //std::srand(std::time(0));
            //Ra::Core::Vector4 clusterColors[10] = {{0.45,0,0,0}, {0.80,0.36,0.36,0}, {1,0.63,0.48,0}, {1,0.55,0,0}, {1,0.84,0,0}, {0.60,0.80,0.20,0}, {0.18,0.55,0.34,0}, {0.37,0.62,0.63,0}, {0.27,0.51,0.71,0}, {0,0,0.55,0}};
            Ra::Core::Vector4 clusterColors[10] = {{0.45,0,0,0}, {0.86,0.64,0.15,0}, {0.10,0.43,0.24,0}, {0.02,0.16,0.25,0}, {1,0.84,0,0}, {0.60,0.80,0.20,0}, {0.18,0.55,0.34,0}, {0.37,0.62,0.63,0}, {0.27,0.51,0.71,0}, {0,0,0.55,0}};
            DistanceSorting::iterator it = m_distSort.begin();
            for (uint i = 0; i < m_nbclusters_display; i++)
            {
                //Ra::Core::Vector4 clusterColor ((Scalar)(rand() % 1000) / (Scalar)1000, (Scalar)(rand() % 1000) / (Scalar)1000, (Scalar)(rand() % 1000) / (Scalar)1000, (Scalar)(rand() % 1000) / (Scalar)1000);
                while ((*it).r <= m_finalClusters3[i] && it != m_distSort.end())
                {
                    MeshContactElement* obj = m_meshContactElements[(*it).objId];
                    Ra::Core::VectorArray<Ra::Core::Triangle> t = obj->getTriangleMeshDuplicate().m_triangles;
                    Ra::Core::Vector4Array colors = obj->getMesh()->getData(Ra::Engine::Mesh::VERTEX_COLOR);

                    colors[t[(*it).faceId][0]] = clusterColors[i];
                    colors[t[(*it).faceId][1]] = clusterColors[i];
                    colors[t[(*it).faceId][2]] = clusterColors[i];
                    obj->getMesh()->addData(Ra::Engine::Mesh::VERTEX_COLOR, colors);

                    std::advance(it, 1);
                }
            }
        }

        void MeshContactManager::setComputeClusters()
        {
            finalDistribCleaned();
            if (m_nbclusters_compute > m_finalDistribCleaned.size() / 2 + 1)
            {
                m_nbclusters_compute = m_finalDistribCleaned.size() / 2 + 1;
            }
            topologicalPersistence();

            //clusterFiltering();
        }

        void MeshContactManager::setDisplayClusters()
        {
            // reloading initial mesh in case of successive simplifications
            for (uint objIndex = 0; objIndex < m_meshContactElements.size(); objIndex++)
            {
                m_meshContactElements[objIndex]->setMesh(m_meshContactElements[objIndex]->getTriangleMeshDuplicate());
            }

//            clustering(0.75,25);
            //colorClusters();
            colorClusters3();
        }


        void MeshContactManager::setConstructM01()
        {
            //m_mainqueue.clear();
            m_index_pmdata.clear();
            m_curr_vsplit = 0;

            m_nbfaces = m_nbfacesinit;
            int nbfaces_scene_init = m_nb_faces_max;

            LOG(logINFO) << "Simplification begins...";

            // end criterion : number of faces set in the UI
            int i = 0;

            //QueueContact::iterator it = m_mainqueue.begin();

            //Ra::Core::PriorityQueue::PriorityQueueContainer::iterator it = m_mainqueue.begin();

            uint n = 99;



            while (!m_mainqueue.empty() && m_nb_faces_max > m_nbfaces)
            {
                //const Ra::Core::PriorityQueue::PriorityQueueData &d = *it;
                const Ra::Core::PriorityQueue::PriorityQueueData &d = m_mainqueue.firstData();
                MeshContactElement* obj = static_cast<MeshContactElement*>(m_meshContactElements[d.m_index]);
                int nbfaces = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getNbFaces();

                if (nbfaces > 2)
                {
                    if (edgeCollapse1(obj->getIndex()))
                    {
                        m_index_pmdata.push_back(obj->getIndex());
                        m_curr_vsplit++;
                        m_nb_faces_max -= (nbfaces - obj->getProgressiveMeshLOD()->getProgressiveMesh()->getNbFaces());
                        //LOG(logINFO) << "Current nb of faces : " << m_nb_faces_max;

                        if (m_nb_faces_max == (nbfaces_scene_init / 100) * n || m_nb_faces_max == (nbfaces_scene_init / 100) * n - 1)
                        {
                            LOG(logINFO) << "LOD : " << n << "%";
                            n--;
                        }
                    }
                }
                i++;
                //m_mainqueue.erase(it);
                //it = m_mainqueue.begin();
            }

            LOG(logINFO) << "Simplification ends...";

            LOG(logINFO) << "Priority queue time : " << m_pqueue_time << " s";

            for (const auto& elem : m_meshContactElements)
            {
                MeshContactElement* obj = static_cast<MeshContactElement*>(elem);

                // switch from DCEL to mesh
                Ra::Core::TriangleMesh newMesh;
                Ra::Core::convertPM(*(obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()), newMesh);
                obj->updateTriangleMesh(newMesh);
            }
        }


        void MeshContactManager::setConstructM0()
        {     
            m_mainqueue.clear();
            m_index_pmdata.clear();
            m_curr_vsplit = 0;

            m_nbfaces = m_nbfacesinit;
            int nbfaces_scene_init = m_nb_faces_max;

            LOG(logINFO) << "Simplification begins...";

            for (uint e = 0; e < m_nbobjects; e++)
            {
                MeshContactElement* obj = m_meshContactElements[e];
                auto start = std::chrono::high_resolution_clock::now();
                m_mainqueue.insert(obj->getPriorityQueue()->firstData());
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<Scalar> elapsed = end - start;
                m_pqueue_time += elapsed.count();
            }

            // end criterion : number of faces set in the UI
            int i = 0;

            //QueueContact::iterator it = m_mainqueue.begin();

            Ra::Core::PriorityQueue::PriorityQueueContainer::iterator it = m_mainqueue.begin();

            uint n = 99;

            while (it != m_mainqueue.end() && m_nb_faces_max > m_nbfaces)
            {
                const Ra::Core::PriorityQueue::PriorityQueueData &d = *it;
                MeshContactElement* obj = static_cast<MeshContactElement*>(m_meshContactElements[d.m_index]);
                int nbfaces = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getNbFaces();

                if (nbfaces > 2)
                {
                    if (edgeCollapse(obj->getIndex()))
                    {
                        m_index_pmdata.push_back(obj->getIndex());
                        m_curr_vsplit++;
                        m_nb_faces_max -= (nbfaces - obj->getProgressiveMeshLOD()->getProgressiveMesh()->getNbFaces());
                        //LOG(logINFO) << "Current nb of faces : " << m_nb_faces_max;

                        if (m_nb_faces_max == (nbfaces_scene_init / 100) * n || m_nb_faces_max == (nbfaces_scene_init / 100) * n - 1)
                        {
                            LOG(logINFO) << "LOD : " << n << "%";
                            n--;
                        }
                    }
                    if (obj->getPriorityQueue()->size() > 0)
                    {
                        auto start = std::chrono::high_resolution_clock::now();
                        m_mainqueue.insert(obj->getPriorityQueue()->firstData());
                        auto end = std::chrono::high_resolution_clock::now();
                        std::chrono::duration<Scalar> elapsed = end - start;
                        m_pqueue_time += elapsed.count();
                    }
                    else
                    {
                        LOG(logINFO) << "Priority queue empty";
                    }
                }
                i++;
                m_mainqueue.erase(it);
                it = m_mainqueue.begin();
            }

            LOG(logINFO) << "Simplification ends...";

            LOG(logINFO) << "Priority queue time : " << m_pqueue_time << " s";

            for (const auto& elem : m_meshContactElements)
            {
                MeshContactElement* obj = static_cast<MeshContactElement*>(elem);

                // switch from DCEL to mesh
                Ra::Core::TriangleMesh newMesh;
                Ra::Core::convertPM(*(obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()), newMesh);
                obj->updateTriangleMesh(newMesh);
            }
        }

        int MeshContactManager::getNbFacesMax()
        {
            computeNbFacesMax2();
            return m_nb_faces_max;
        }

        bool MeshContactManager::edgeErrorComputation(Ra::Core::HalfEdge_ptr h, int objIndex, Scalar& error, Ra::Core::Vector3& p)
        {
            const Ra::Core::Vertex_ptr& vs = h->V();
            const Ra::Core::Vertex_ptr& vt = h->Next()->V();

            MeshContactElement* obj = static_cast<MeshContactElement*>(m_meshContactElements[objIndex]);

            // test if the edge can be collapsed or if it has contact
            bool contact = false;
            Ra::Core::ProgressiveMesh<>::Primitive qk;
            Scalar dist;
            Scalar weight;
            int nbContacts = 0;
            Scalar sumWeight = 0;

            // for each edge, we look for all contacts with other objects and add the contact quadrics to the quadric of the edge
            Ra::Core::ProgressiveMesh<>::Primitive qc = Ra::Core::ProgressiveMesh<>::Primitive();
            if (m_lambda != 0.0)
            {
                /// Proximity-aware QEM
                if (m_proximity)
                {
                    std::vector<Ra::Core::ProgressiveMesh<>::Primitive> proximities;
                    std::mutex mutex;

                    #pragma omp parallel for
                    for (uint k=0; k<m_trianglekdtrees.size(); k++)
                    {
                        //if (k != objIndex)
                        if (m_proximityPairs2(objIndex,k))
                        {
                            MeshContactElement* otherObj = static_cast<MeshContactElement*>(m_meshContactElements[k]);
                            std::vector<std::pair<int,Scalar> > faceIndexes;

                            // all close faces
                            obj->getProgressiveMeshLOD()->getProgressiveMesh()->edgeContacts(vs->idx, vt->idx, m_trianglekdtrees, k, std::pow(m_broader_threshold,2), faceIndexes);
                            if ( faceIndexes.size() != 0)
                            {
                                for (uint l = 0; l < faceIndexes.size(); l++)
                                {
                                    dist = faceIndexes[l].second;

                                    // asymmetry computation
                                    // Scalar dist2 = m_distances[k][objIndex][faceIndexes[l].first].second;
                                    // if (abs(dist - dist2) <= m_asymmetry)
                                    // {
                                    contact = true;
                                    if (m_broader_threshold == 0.0)
                                    {
                                        CORE_ASSERT(dist == 0.0, "Contact found out of threshold limit");
                                        weight = 1;
                                    }
                                    else
                                    {
                                        CORE_ASSERT(dist/m_broader_threshold >= 0 && dist/m_broader_threshold <= 1, "Contact found out of threshold limit.");
                                        weight = std::pow(std::pow(dist/m_broader_threshold, m_m) - 1, m_n);
                                        //weight = 1;
                                    }
                                    nbContacts++;
                                    sumWeight += weight;
                                    qk = otherObj->getFacePrimitive(faceIndexes[l].first);
                                    qk *= weight;                      
                                    mutex.lock();
                                    proximities.push_back(qk);
                                    mutex.unlock();
                                    //qc += qk;
                                }
                            }
                        }
                    }

                    for (uint p = 0; p < proximities.size(); p++)
                    {
                        qc += proximities[p];
                    }
                }

                /// Proximity detection, one is enough (for weighting qem)
                else
                {
                    uint k = 0;
                    while(k<m_trianglekdtrees.size() && !contact)
                    {
                        if (k != objIndex)
                        {
                            if (obj->getProgressiveMeshLOD()->getProgressiveMesh()->isProximity(vs->idx, vt->idx, m_trianglekdtrees, k, std::pow(m_broader_threshold,2)))
                            {
                                contact = true;
                            }
                        }
                        k++;
                    }
                }
            }

            // computing the optimal placement for the resulting vertex
            Scalar edgeErrorQEM = obj->getProgressiveMeshLOD()->getProgressiveMesh()->computeEdgeError(h->idx, p);

            if (contact)
            {
                /// Proximity-aware QEM
                if (m_proximity)
                {
                    qc *= 1.0 / nbContacts; // normalization using the number og proximities instead of the sum of their weights

                    Scalar edgeErrorContact = abs(obj->getProgressiveMeshLOD()->getProgressiveMesh()->getEM().computeGeometricError(qc,p));

                    //Ra::Core::Vector3 m = (vs->P() + vt->P()) / 2;
                    //Scalar edgeErrorContact_init = abs(obj->getProgressiveMeshLOD()->getProgressiveMesh()->getEM().computeGeometricError(qc,m));

//                    Scalar edgeErrorContact_vs = abs(obj->getProgressiveMeshLOD()->getProgressiveMesh()->getEM().computeGeometricError(qc,vs->P()));
//                    Scalar edgeErrorContact_vt = abs(obj->getProgressiveMeshLOD()->getProgressiveMesh()->getEM().computeGeometricError(qc,vt->P()));
//                    Scalar edgeErrorContact_e = (edgeErrorContact_vs + edgeErrorContact_vt) / 2;

                    error = edgeErrorQEM * (1 + m_lambda * abs(edgeErrorContact /*- edgeErrorContact_e*/));
                    CORE_ASSERT(error >= edgeErrorQEM, "Contacts lower the error");
                }

                /// Weighting QEM
                else
                {
                    error = edgeErrorQEM * m_weight;
                }
            }
            else
            {
                error = edgeErrorQEM;
            }
            return contact;
        }


        /// A unique priority queue for the scene
        void MeshContactManager::constructPriorityQueues1()
        {

            for (uint objIndex=0; objIndex < m_meshContactElements.size(); objIndex++)
            {
                Ra::Core::ProgressiveMeshBase<>* pm = new Ra::Core::ProgressiveMesh<>(&m_initTriangleMeshes[objIndex]);
                m_meshContactElements[objIndex]->setProgressiveMeshLOD(pm);
                m_meshContactElements[objIndex]->getProgressiveMeshLOD()->getProgressiveMesh()->computeFacesQuadrics();
                m_meshContactElements[objIndex]->computeFacePrimitives();
                // reloading the mesh in case of successive simplifications
                m_meshContactElements[objIndex]->setMesh(m_initTriangleMeshes[objIndex]);
            }

            /// Boundary method : finding boundary vertices in all meshes
            if (m_boundary)
            {
                LOG(logINFO) << "Boundary vertices computation begins...";
                for (uint objIndex=0; objIndex < m_meshContactElements.size(); objIndex++)
                {
                    std::vector<Ra::Core::Index> boundary;
                    int nb = m_meshContactElements[objIndex]->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_vertex.size();
                    for (uint i = 0; i < nb; i++)
                    {
                        Ra::Core::Vertex_ptr& v = m_meshContactElements[objIndex]->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_vertex[i];
                        uint k = 0;
                        bool contact = false;
                        while(k<m_trianglekdtrees.size() && !contact)
                        {
                            if (m_proximityPairs2(objIndex,k))
                            {
                                if (m_meshContactElements[objIndex]->getProgressiveMeshLOD()->getProgressiveMesh()->isProximityVertex(v->idx, m_trianglekdtrees, k, std::pow(m_broader_threshold,2)))
                                {
                                    contact = true;
                                }
                            }
                            k++;
                        }
                        if (contact)
                        {
                            boundary.push_back(v->idx);
                        }
                    }
                    m_boundaryVertices.push_back(boundary);
                }
                LOG(logINFO) << "Boundary vertices computation ends...";
            }

            LOG(logINFO) << "Priority queue computation begins...";

            for (uint objIndex=0; objIndex < m_nbobjects; objIndex++)
            {
            MeshContactElement* obj = static_cast<MeshContactElement*>(m_meshContactElements[objIndex]);

            const uint numTriangles = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_face.size();

            Ra::Core::Vector4 vertexColor (0, 0, 0, 0);
            int nbVertices = obj->getMesh()->getGeometry().m_vertices.size();
            Ra::Core::Vector4Array colors;
            for (uint v = 0; v < nbVertices; v++)
            {
                colors.push_back(vertexColor);
            }
            Ra::Core::Vector4 contactColor (0.45,0,0,0);

//#pragma omp parallel for
            for (unsigned int i = 0; i < numTriangles; i++)
            {

                // browse edges
                Ra::Core::Vector3 p = Ra::Core::Vector3::Zero();
                int j;
                const Ra::Core::Face_ptr& f = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_face.at( i );
                Ra::Core::HalfEdge_ptr h = f->HE();
                for (j = 0; j < 3; j++)
                {
                    const Ra::Core::Vertex_ptr& vs = h->V();
                    const Ra::Core::Vertex_ptr& vt = h->Next()->V();

                    // to prevent adding twice the same edge
                    if (vs->idx > vt->idx)
                    {
                        h = h->Next();
                        continue;
                    }

                    Scalar error;
                    bool contact = edgeErrorComputation(h, objIndex, error, p);

                    // coloring proximity zones
                    if (contact)
                    {
                        colors[vs->idx] = contactColor;
                        colors[vt->idx] = contactColor;
                    }

                    // insert into the priority queue with the real resulting point
//#pragma omp critical
                    {
                        auto start = std::chrono::high_resolution_clock::now();
                        m_mainqueue.insert(Ra::Core::PriorityQueue::PriorityQueueData(vs->idx, vt->idx, h->idx, i, error, p, objIndex));
                        auto end = std::chrono::high_resolution_clock::now();
                        std::chrono::duration<Scalar> elapsed = end - start;
                        m_pqueue_time += elapsed.count();
                    }

                    h = h->Next();
                }
            }
            obj->getMesh()->addData(Ra::Engine::Mesh::VERTEX_COLOR, colors);

            }

            LOG(logINFO) << "Priority queue computation ends...";
            LOG(logINFO) << "Size : " << m_mainqueue.size();
        }

        void MeshContactManager::updatePriorityQueue1(Ra::Core::Index vsIndex, Ra::Core::Index vtIndex, int objIndex)
        {
            MeshContactElement* obj = static_cast<MeshContactElement*>(m_meshContactElements[objIndex]);

            auto start = std::chrono::high_resolution_clock::now();
            m_mainqueue.removeEdges(vsIndex,objIndex);
            m_mainqueue.removeEdges(vtIndex,objIndex);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<Scalar> elapsed = end - start;
            m_pqueue_time += elapsed.count();

            Ra::Core::Vector3 p = Ra::Core::Vector3::Zero();

            // listing of all the new edges formed with vs
            Ra::Core::VHEIterator vsHEIt = Ra::Core::VHEIterator(obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_vertex[vsIndex]);
            Ra::Core::HalfEdgeList adjHE = vsHEIt.list();

            // test if the other vertex of the edge has any contacts
            for (uint i = 0; i < adjHE.size(); i++)
            {
                Ra::Core::HalfEdge_ptr h = adjHE[i];

                const Ra::Core::Vertex_ptr& vs = h->V();
                const Ra::Core::Vertex_ptr& vt = h->Next()->V();

                Scalar error;
                edgeErrorComputation(h, objIndex, error, p);

                // insert into the priority queue with the real resulting point
                // check that the index of the starting point of the edge is smaller than the index of its ending point
                if (vs->idx < vt->idx)
                {
                    start = std::chrono::high_resolution_clock::now();
                    m_mainqueue.insert(Ra::Core::PriorityQueue::PriorityQueueData(vs->idx, vt->idx, h->idx, h->F()->idx, error, p, objIndex));
                    end = std::chrono::high_resolution_clock::now();
                    elapsed = end - start;
                    m_pqueue_time += elapsed.count();
                }
                else
                {
                    start = std::chrono::high_resolution_clock::now();
                    m_mainqueue.insert(Ra::Core::PriorityQueue::PriorityQueueData(vt->idx, vs->idx, h->Twin()->idx, h->Twin()->F()->idx, error, p, objIndex));
                    end = std::chrono::high_resolution_clock::now();
                    elapsed = end - start;
                    m_pqueue_time += elapsed.count();
                }
            }
        }


        /// A priority queue per mesh + global priority queue
        void MeshContactManager::constructPriorityQueues2()
        {

            for (uint objIndex=0; objIndex < m_meshContactElements.size(); objIndex++)
            {
                Ra::Core::ProgressiveMeshBase<>* pm = new Ra::Core::ProgressiveMesh<>(&m_initTriangleMeshes[objIndex]);
                m_meshContactElements[objIndex]->setProgressiveMeshLOD(pm);
                m_meshContactElements[objIndex]->getProgressiveMeshLOD()->getProgressiveMesh()->computeFacesQuadrics();
                m_meshContactElements[objIndex]->computeFacePrimitives();
                // reloading the mesh in case of successive simplifications
                m_meshContactElements[objIndex]->setMesh(m_initTriangleMeshes[objIndex]);
            }

            /// Boundary method : finding boundary vertices in all meshes
            if (m_boundary)
            {
                LOG(logINFO) << "Boundary vertices computation begins...";
                for (uint objIndex=0; objIndex < m_meshContactElements.size(); objIndex++)
                {
                    std::vector<Ra::Core::Index> boundary;
                    int nb = m_meshContactElements[objIndex]->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_vertex.size();
                    for (uint i = 0; i < nb; i++)
                    {
                        Ra::Core::Vertex_ptr& v = m_meshContactElements[objIndex]->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_vertex[i];
                        uint k = 0;
                        bool contact = false;
                        while(k<m_trianglekdtrees.size() && !contact)
                        {
                            if (m_proximityPairs2(objIndex,k))
                            {
                                if (m_meshContactElements[objIndex]->getProgressiveMeshLOD()->getProgressiveMesh()->isProximityVertex(v->idx, m_trianglekdtrees, k, std::pow(m_broader_threshold,2)))
                                {
                                    contact = true;
                                }
                            }
                            k++;
                        }
                        if (contact)
                        {
                            boundary.push_back(v->idx);
                        }
                    }
                    m_boundaryVertices.push_back(boundary);
                }
                LOG(logINFO) << "Boundary vertices computation ends...";
            }

            LOG(logINFO) << "Priority queue computation begins...";

            for (uint objIndex=0; objIndex < m_nbobjects; objIndex++)
            {
            MeshContactElement* obj = static_cast<MeshContactElement*>(m_meshContactElements[objIndex]);

            Ra::Core::PriorityQueue pQueue = Ra::Core::PriorityQueue();
            const uint numTriangles = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_face.size();

            Ra::Core::Vector4 vertexColor (0, 0, 0, 0);
            int nbVertices = obj->getMesh()->getGeometry().m_vertices.size();
            Ra::Core::Vector4Array colors;
            for (uint v = 0; v < nbVertices; v++)
            {
                colors.push_back(vertexColor);
            }
            Ra::Core::Vector4 contactColor (0.45,0,0,0);

//#pragma omp parallel for
            for (unsigned int i = 0; i < numTriangles; i++)
            {

                // browse edges
                Ra::Core::Vector3 p = Ra::Core::Vector3::Zero();
                int j;
                const Ra::Core::Face_ptr& f = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_face.at( i );
                Ra::Core::HalfEdge_ptr h = f->HE();
                for (j = 0; j < 3; j++)
                {
                    const Ra::Core::Vertex_ptr& vs = h->V();
                    const Ra::Core::Vertex_ptr& vt = h->Next()->V();

                    // to prevent adding twice the same edge
                    if (vs->idx > vt->idx)
                    {
                        h = h->Next();
                        continue;
                    }

                    Scalar error;
                    bool contact = edgeErrorComputation(h, objIndex, error, p);

                    // coloring proximity zones
                    if (contact)
                    {
                        colors[vs->idx] = contactColor;
                        colors[vt->idx] = contactColor;
                    }

                    // insert into the priority queue with the real resulting point
//#pragma omp critical
                    {
                        auto start = std::chrono::high_resolution_clock::now();
                        pQueue.insert(Ra::Core::PriorityQueue::PriorityQueueData(vs->idx, vt->idx, h->idx, i, error, p, objIndex));
                        auto end = std::chrono::high_resolution_clock::now();
                        std::chrono::duration<Scalar> elapsed = end - start;
                        m_pqueue_time += elapsed.count();
                    }

                    h = h->Next();
                }
            }
            obj->getMesh()->addData(Ra::Engine::Mesh::VERTEX_COLOR, colors);

            obj->setPriorityQueue(pQueue);

            }

            LOG(logINFO) << "Priority queue computation ends...";
        }

        void MeshContactManager::updatePriorityQueue2(Ra::Core::Index vsIndex, Ra::Core::Index vtIndex, int objIndex)
        {
            MeshContactElement* obj = static_cast<MeshContactElement*>(m_meshContactElements[objIndex]);
            auto start = std::chrono::high_resolution_clock::now();
            obj->getPriorityQueue()->removeEdges(vsIndex,objIndex);
            obj->getPriorityQueue()->removeEdges(vtIndex,objIndex);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<Scalar> elapsed = end - start;
            m_pqueue_time += elapsed.count();

            Ra::Core::Vector3 p = Ra::Core::Vector3::Zero();

            // listing of all the new edges formed with vs
            Ra::Core::VHEIterator vsHEIt = Ra::Core::VHEIterator(obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_vertex[vsIndex]);
            Ra::Core::HalfEdgeList adjHE = vsHEIt.list();

            // test if the other vertex of the edge has any contacts
            for (uint i = 0; i < adjHE.size(); i++)
            {
                Ra::Core::HalfEdge_ptr h = adjHE[i];

                const Ra::Core::Vertex_ptr& vs = h->V();
                const Ra::Core::Vertex_ptr& vt = h->Next()->V();

                Scalar error;
                edgeErrorComputation(h, objIndex, error, p);

                // insert into the priority queue with the real resulting point
                // check that the index of the starting point of the edge is smaller than the index of its ending point
                if (vs->idx < vt->idx)
                {
                    start = std::chrono::high_resolution_clock::now();
                    obj->getPriorityQueue()->insert(Ra::Core::PriorityQueue::PriorityQueueData(vs->idx, vt->idx, h->idx, h->F()->idx, error, p, objIndex));
                    end = std::chrono::high_resolution_clock::now();
                    elapsed = end - start;
                    m_pqueue_time += elapsed.count();
                }
                else
                {
                    start = std::chrono::high_resolution_clock::now();
                    obj->getPriorityQueue()->insert(Ra::Core::PriorityQueue::PriorityQueueData(vt->idx, vs->idx, h->Twin()->idx, h->Twin()->F()->idx, error, p, objIndex));
                    end = std::chrono::high_resolution_clock::now();
                    elapsed = end - start;
                    m_pqueue_time += elapsed.count();
                }
            }
        }

        bool MeshContactManager::isConstructM0(int objIndex)
        {
            MeshContactElement* obj = static_cast<MeshContactElement*>(m_meshContactElements[objIndex]);
            Ra::Core::PriorityQueue::PriorityQueueData d = m_mainqueue.firstData();
            Ra::Core::HalfEdge_ptr he = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_halfedge[d.m_edge_id];
            if (! obj->getProgressiveMeshLOD()->getProgressiveMesh()->isEcolConsistent(he->idx, d.m_p_result))
            {
                m_mainqueue.top();
                return false;
            }
            else if (! obj->getProgressiveMeshLOD()->getProgressiveMesh()->isEcolPossible(he->idx, d.m_p_result))
            {
                m_mainqueue.top();
                return false;
            }
            else
                return true;
        }

        bool MeshContactManager::edgeCollapse1(int objIndex)
        {
            MeshContactElement* obj = static_cast<MeshContactElement*>(m_meshContactElements[objIndex]);

            if (isConstructM0(objIndex))
            {
                // edge collapse and putting the collapse data in the ProgressiveMeshLOD
                Ra::Core::PriorityQueue::PriorityQueueData d = m_mainqueue.top();

                /// Boundary method : if the edge has only one boundary vertex, it will be the resulting vertex (halfedge collapse)
                if (m_boundary)
                {
                    bool vsIsBoundary = false;
                    bool vtIsBoundary = false;

                    int nb = m_boundaryVertices[objIndex].size();

                    uint i = 0;

                    while ((!vsIsBoundary || !vtIsBoundary) && i < nb)
                    {
                        if (m_boundaryVertices[objIndex][i] == d.m_vs_id)
                        {
                            vsIsBoundary = true;
                        }
                        else if (m_boundaryVertices[objIndex][i] == d.m_vt_id)
                        {
                            vtIsBoundary = true;
                        }
                        i++;
                    }

                    if (vsIsBoundary && !vtIsBoundary)
                    {
                        d.m_p_result = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_vertex[d.m_vs_id]->P();
                        //LOG(logINFO) << "Halfedge collapse";
                    }
                    else if (!vsIsBoundary && vtIsBoundary)
                    {
                        d.m_p_result = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_vertex[d.m_vt_id]->P();
                        m_boundaryVertices[objIndex].push_back(d.m_vs_id);
                        //LOG(logINFO) << "Halfedge collapse";
                    }
                }

                Ra::Core::HalfEdge_ptr he = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_halfedge[d.m_edge_id];


                if (he->Twin() == nullptr)
                {
                    obj->getProgressiveMeshLOD()->getProgressiveMesh()->collapseFace();
                    obj->getProgressiveMeshLOD()->oneVertexSplitPossible();
                }
                else
                {
                    obj->getProgressiveMeshLOD()->getProgressiveMesh()->collapseFace();
                    obj->getProgressiveMeshLOD()->getProgressiveMesh()->collapseFace();
                }
                obj->getProgressiveMeshLOD()->getProgressiveMesh()->collapseVertex();

                Ra::Core::ProgressiveMeshData data = Ra::Core::DcelOperations::edgeCollapse(*(obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()), d.m_edge_id, d.m_p_result);

                if (obj->getProgressiveMeshLOD()->getProgressiveMesh()->getNbFaces() > 0)
                {
                    obj->getProgressiveMeshLOD()->getProgressiveMesh()->updateFacesQuadrics(d.m_vs_id);
                }
                // update the priority queue of the object
                // updatePriorityQueue(d.m_vs_id, d.m_vt_id, objIndex);
                //updatePriorityQueue2(d.m_vs_id, d.m_vt_id, objIndex);
                updatePriorityQueue1(d.m_vs_id, d.m_vt_id, objIndex);
    //            else
    //            {
    //                while (obj->getPriorityQueue()->size() > 0)
    //                    obj->getPriorityQueue()->top();
    //            }
                obj->getProgressiveMeshLOD()->addData(data);
                obj->getProgressiveMeshLOD()->oneEdgeCollapseDone();

                return true;
            }
            else
            {
                return false;
            }
        }


        bool MeshContactManager::edgeCollapse(int objIndex)
        {
            MeshContactElement* obj = static_cast<MeshContactElement*>(m_meshContactElements[objIndex]);

            if (obj->isConstructM0())
            {
                // edge collapse and putting the collapse data in the ProgressiveMeshLOD
                Ra::Core::PriorityQueue::PriorityQueueData d = obj->getPriorityQueue()->top();

                /// Boundary method : if the edge has only one boundary vertex, it will be the resulting vertex (halfedge collapse)
                if (m_boundary)
                {
                    bool vsIsBoundary = false;
                    bool vtIsBoundary = false;

                    int nb = m_boundaryVertices[objIndex].size();

                    uint i = 0;

                    while ((!vsIsBoundary || !vtIsBoundary) && i < nb)
                    {
                        if (m_boundaryVertices[objIndex][i] == d.m_vs_id)
                        {
                            vsIsBoundary = true;
                        }
                        else if (m_boundaryVertices[objIndex][i] == d.m_vt_id)
                        {
                            vtIsBoundary = true;
                        }
                        i++;
                    }

                    if (vsIsBoundary && !vtIsBoundary)
                    {
                        d.m_p_result = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_vertex[d.m_vs_id]->P();
                        //LOG(logINFO) << "Halfedge collapse";
                    }
                    else if (!vsIsBoundary && vtIsBoundary)
                    {
                        d.m_p_result = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_vertex[d.m_vt_id]->P();
                        m_boundaryVertices[objIndex].push_back(d.m_vs_id);
                        //LOG(logINFO) << "Halfedge collapse";
                    }
                }

                Ra::Core::HalfEdge_ptr he = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_halfedge[d.m_edge_id];


                if (he->Twin() == nullptr)
                {
                    obj->getProgressiveMeshLOD()->getProgressiveMesh()->collapseFace();
                    obj->getProgressiveMeshLOD()->oneVertexSplitPossible();
                }
                else
                {
                    obj->getProgressiveMeshLOD()->getProgressiveMesh()->collapseFace();
                    obj->getProgressiveMeshLOD()->getProgressiveMesh()->collapseFace();
                }
                obj->getProgressiveMeshLOD()->getProgressiveMesh()->collapseVertex();

                Ra::Core::ProgressiveMeshData data = Ra::Core::DcelOperations::edgeCollapse(*(obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()), d.m_edge_id, d.m_p_result);

                if (obj->getProgressiveMeshLOD()->getProgressiveMesh()->getNbFaces() > 0)
                {
                    obj->getProgressiveMeshLOD()->getProgressiveMesh()->updateFacesQuadrics(d.m_vs_id);
                }
                // update the priority queue of the object
                // updatePriorityQueue(d.m_vs_id, d.m_vt_id, objIndex);
                updatePriorityQueue2(d.m_vs_id, d.m_vt_id, objIndex);
                //updatePriorityQueue1(d.m_vs_id, d.m_vt_id, objIndex);
    //            else
    //            {
    //                while (obj->getPriorityQueue()->size() > 0)
    //                    obj->getPriorityQueue()->top();
    //            }
                obj->getProgressiveMeshLOD()->addData(data);
                obj->getProgressiveMeshLOD()->oneEdgeCollapseDone();

                return true;
            }
            else
            {
                return false;
            }
        }

        void MeshContactManager::colorMeshes()
        {

            Ra::Core::Vector4 vertexColor[17] = {{0.45,0,0,0}, {0.02,0.16,0.25,0}, {0.15,0,0,0},
                                                {0.46,0.25,0.01,0}, {0.11,0.40,0.65,0}, {0.45,0.64,0.75,0},
                                                {0,0.45,0.25,0}, {0.95,0.63,0.02,0}, {0.67,0.65,0.75,0},
                                                {0.35,0.34,0.46,0}, {0.35,0.24,0.18,0}, {0.75,0.60,0.56,0},
                                                {0.64,0.35,0.43,0}, {0.36,0.29,0.45,0}, {0.95,0.69,0.35,0},
                                                {0.96,0.53,0.29,0}, {0.96,0.42,0.31,0}};

            for (uint i = 0; i < m_meshContactElements.size(); i++)
            {
                m_meshContactElements[i]->setMesh(m_meshContactElements[i]->getTriangleMeshDuplicate());

                MeshContactElement* obj = m_meshContactElements[i];
                int nbVertices = obj->getMesh()->getGeometry().m_vertices.size();
                Ra::Core::Vector4Array colors;
                for (uint v = 0; v < nbVertices; v++)
                {
                    colors.push_back(vertexColor[i]);
                }
                obj->getMesh()->addData(Ra::Engine::Mesh::VERTEX_COLOR, colors);
            }
        }

        void MeshContactManager::colorMeshesSimplified()
        {

            Ra::Core::Vector4 vertexColor[17] = {{0.45,0,0,0}, {0.02,0.16,0.25,0}, {0.15,0,0,0},
                                                {0.46,0.25,0.01,0}, {0.11,0.40,0.65,0}, {0.45,0.64,0.75,0},
                                                {0,0.45,0.25,0}, {0.95,0.63,0.02,0}, {0.67,0.65,0.75,0},
                                                {0.35,0.34,0.46,0}, {0.35,0.24,0.18,0}, {0.75,0.60,0.56,0},
                                                {0.64,0.35,0.43,0}, {0.36,0.29,0.45,0}, {0.95,0.69,0.35,0},
                                                {0.96,0.53,0.29,0}, {0.96,0.42,0.31,0}};

            for (uint i = 0; i < m_meshContactElements.size(); i++)
            {
                m_meshContactElements[i]->setMesh(m_meshContactElements[i]->getTriangleMeshDuplicateSimplified());

                MeshContactElement* obj = m_meshContactElements[i];
                int nbVertices = obj->getMesh()->getGeometry().m_vertices.size();
                Ra::Core::Vector4Array colors;
                for (uint v = 0; v < nbVertices; v++)
                {
                    colors.push_back(vertexColor[i]);
                }
                obj->getMesh()->addData(Ra::Engine::Mesh::VERTEX_COLOR, colors);
            }

            //HausdorffDistance();
        }

        void MeshContactManager::setWeightState(bool state)
        {
            m_proximity = state;
        }

        void MeshContactManager::setWeight(double weight)
        {
            m_weight = weight;
        }

        void MeshContactManager::setBoundary()
        {
            m_proximity = false;
            m_boundary = true;
        }

        void MeshContactManager::proxVertices()
        {
            for (uint i = 0; i < m_meshContactElements.size(); i++)
            {
                IndexSorting prox;

                for (uint k = 0; k < m_initTriangleMeshes[i].m_vertices.size(); k++)
                {
                    bool find = false;
                    uint j = 0;
                    MeshContactElement* obj;
                    while (!find && j < m_meshContactElements.size())
                    {
                        if (m_proximityPairs2(i,j))
                        {
                            std::pair<Ra::Core::Index,Scalar> triangle = m_trianglekdtrees[j]->doQueryRestrictedClosestIndexPoint(m_initTriangleMeshes[i].m_vertices[k]);
                            if (triangle.second <= m_threshold)
                            {
                                prox.insert(k);
                                find = true;
                            }
                        }
                        j++;
                    }
                }

                m_proxVertices.push_back(prox);

                LOG(logINFO) << "Obj " << i << " proximities found.";
            }
        }

        int MeshContactManager::findVertex(Ra::Core::Vector3 vertex, const Ra::Core::VectorArray<Ra::Core::Vector3>& vertices)
        {
            for (int i = 0; i < vertices.size(); i++)
            {
                if (vertices[i] == vertex)
                {
                    return i;
                }
            }
            return -1;
        }

        void MeshContactManager::midpointSubdivision()
        {
            // Initializing the triangle meshes in case of successive simplifications
            m_midptTriangleMeshes.clear();
            m_midptTrianglekdtrees.clear();

            int i = 0;

            for (auto& obj : m_meshContactElements)
            {
                Ra::Core::TriangleMesh tm = obj->getMesh()->getGeometry();
                Ra::Core::TriangleMesh tmSub;
                tmSub.m_normals = tm.m_normals;
                tmSub.m_vertices = tm.m_vertices;

                for (auto& triangle : tm.m_triangles)
                {
                    uint v0 = triangle[0];
                    uint v1 = triangle[1];
                    uint v2 = triangle[2];

                    uint v3, v4, v5;

                    // avoiding duplicate mid points of edges
                    if (findVertex((tm.m_vertices[v0] + tm.m_vertices[v1]) / 2, tmSub.m_vertices) != -1)
                    {
                        v3 = findVertex((tm.m_vertices[v0] + tm.m_vertices[v1]) / 2, tmSub.m_vertices);
                    }
                    else
                    {
                        tmSub.m_vertices.push_back((tm.m_vertices[v0] + tm.m_vertices[v1]) / 2);
                        v3 = tmSub.m_vertices.size() - 1;
                    }
                    if (findVertex((tm.m_vertices[v1] + tm.m_vertices[v2]) / 2, tmSub.m_vertices) != -1)
                    {
                        v4 = findVertex((tm.m_vertices[v1] + tm.m_vertices[v2]) / 2, tmSub.m_vertices);
                    }
                    else
                    {
                        tmSub.m_vertices.push_back((tm.m_vertices[v1] + tm.m_vertices[v2]) / 2);
                        v4 = tmSub.m_vertices.size() - 1;
                    }
                    if (findVertex((tm.m_vertices[v2] + tm.m_vertices[v0]) / 2, tmSub.m_vertices) != -1)
                    {
                        v5 = findVertex((tm.m_vertices[v2] + tm.m_vertices[v0]) / 2, tmSub.m_vertices);
                    }
                    else
                    {
                        tmSub.m_vertices.push_back((tm.m_vertices[v2] + tm.m_vertices[v0]) / 2);
                        v5 = tmSub.m_vertices.size() - 1;
                    }

                    tmSub.m_triangles.push_back({v0,v3,v5});
                    tmSub.m_triangles.push_back({v1,v4,v3});
                    tmSub.m_triangles.push_back({v2,v5,v4});
                    tmSub.m_triangles.push_back({v3,v4,v5});

                    tmSub.m_vertices.push_back(Ra::Core::Geometry::triangleBarycenter(tmSub.m_vertices[v0],tmSub.m_vertices[v3],tmSub.m_vertices[v5]));
                    tmSub.m_vertices.push_back(Ra::Core::Geometry::triangleBarycenter(tmSub.m_vertices[v1],tmSub.m_vertices[v4],tmSub.m_vertices[v3]));
                    tmSub.m_vertices.push_back(Ra::Core::Geometry::triangleBarycenter(tmSub.m_vertices[v2],tmSub.m_vertices[v5],tmSub.m_vertices[v4]));
                    tmSub.m_vertices.push_back(Ra::Core::Geometry::triangleBarycenter(tmSub.m_vertices[v3],tmSub.m_vertices[v4],tmSub.m_vertices[v5]));
                }

                m_midptTriangleMeshes.push_back(tmSub);

                Super4PCS::TriangleKdTree<>* trianglekdtree = new Super4PCS::TriangleKdTree<>();
                m_midptTrianglekdtrees.push_back(trianglekdtree);
                m_midptTrianglekdtrees[m_midptTrianglekdtrees.size()-1] = obj->computeTriangleKdTree(m_midptTriangleMeshes[m_midptTriangleMeshes.size()-1]);

                LOG(logINFO) << "Obj " << i << " subdivided.";
                i++;
            }
        }

        void MeshContactManager::HausdorffDistance()
        {
            // Initializing the Hausdorff distances in case of successive simplifications
            m_hausdorffDistances.clear();

            midpointSubdivision();

            if (m_proxVertices.size() == 0)
            {
                proxVertices();
            }

            bool prox;

            Scalar mean_prox;
            Scalar max_prox = 0;
            Scalar mean_non_prox;
            Scalar max_non_prox = 0;

            // All vertices means
            Scalar mean_prox_IS = 0;
            Scalar mean_non_prox_IS = 0;
            Scalar mean_prox_SI = 0;
            Scalar mean_non_prox_SI = 0;

            Scalar nbProxVertices_IS = 0;
            Scalar nbProxVertices_SI = 0;

            Scalar nbVertices = 0;

            for (uint i = 0; i < m_meshContactElements.size(); i++)
            {
                LOG(logINFO) << "Mesh " << i << " : " << m_proxVertices[i].size() << " proximity vertices";

                nbVertices += m_initTriangleMeshes[i].m_vertices.size();

                std::pair<Ra::Core::Vector4,Ra::Core::Vector4> hausdorffDistances;

                Scalar initSimpProxMean = 0;
                Scalar initSimpProxMax = 0;
                Scalar initSimpNonProxMean = 0;
                Scalar initSimpNonProxMax = 0;

                for (uint j = 0; j < m_initTriangleMeshes[i].m_vertices.size(); j++)
                {
                    if (m_proxVertices[i].find(j) != m_proxVertices[i].end())
                    {
                        prox = true;
                        nbProxVertices_IS++;
                    }
                    else
                    {
                        prox = false;
                    }

                    std::pair<Ra::Core::Index,Scalar> triangle = m_midptTrianglekdtrees[i]->doQueryRestrictedClosestIndexPoint(m_initTriangleMeshes[i].m_vertices[j]);

                    if (prox)
                    {
                        initSimpProxMean += triangle.second;
                        mean_prox_IS += triangle.second;
                        if (triangle.second > initSimpProxMax)
                        {
                            initSimpProxMax = triangle.second;
                        }
                    }

                    else
                    {
                        initSimpNonProxMean += triangle.second;
                        mean_non_prox_IS += triangle.second;
                        if (triangle.second > initSimpNonProxMax)
                        {
                            initSimpNonProxMax = triangle.second;
                        }
                    }
                }

                if (initSimpProxMean > 0)
                {
                    initSimpProxMean = initSimpProxMean / m_proxVertices[i].size();
                }
                if (initSimpNonProxMean > 0)
                {
                    initSimpNonProxMean = initSimpNonProxMean / (m_initTriangleMeshes[i].m_vertices.size() - m_proxVertices[i].size());
                }

                hausdorffDistances.first = {initSimpProxMean, initSimpProxMax, initSimpNonProxMean, initSimpNonProxMax};

                Scalar simpInitProxMean = 0;
                Scalar simpInitProxMax = 0;
                Scalar simpInitNonProxMean = 0;
                Scalar simpInitNonProxMax = 0;

                int nbProxVerticesObj = 0;

                for (uint j = 0; j < m_midptTriangleMeshes[i].m_vertices.size(); j++)
                {
                    std::pair<Ra::Core::Index,Scalar> triangle = m_trianglekdtrees[i]->doQueryRestrictedClosestIndexPoint(m_midptTriangleMeshes[i].m_vertices[j]);

                    uint a = m_initTriangleMeshes[i].m_triangles[triangle.first][0];
                    uint b = m_initTriangleMeshes[i].m_triangles[triangle.first][1];
                    uint c = m_initTriangleMeshes[i].m_triangles[triangle.first][2];

                    // prox if an edge of the triangle is at prox

                    if (m_proxVertices[i].find(a) != m_proxVertices[i].end())
                    {
                        if (m_proxVertices[i].find(b) != m_proxVertices[i].end() || m_proxVertices[i].find(c) != m_proxVertices[i].end())
                        {
                            prox = true;
                            nbProxVerticesObj++;
                            nbProxVertices_SI++;
                        }
                    }
                    else if (m_proxVertices[i].find(b) != m_proxVertices[i].end())
                    {
                        if (m_proxVertices[i].find(a) != m_proxVertices[i].end() || m_proxVertices[i].find(c) != m_proxVertices[i].end())
                        {
                            prox = true;
                            nbProxVerticesObj++;
                            nbProxVertices_SI++;
                        }
                    }
                    else if (m_proxVertices[i].find(c) != m_proxVertices[i].end())
                    {
                        if (m_proxVertices[i].find(a) != m_proxVertices[i].end() || m_proxVertices[i].find(b) != m_proxVertices[i].end())
                        {
                            prox = true;
                            nbProxVerticesObj++;
                            nbProxVertices_SI++;
                        }
                    }
                    else
                    {
                        prox = false;
                    }

                    if (prox)
                    {
                        simpInitProxMean += triangle.second;
                        mean_prox_SI += triangle.second;
                        if (triangle.second > simpInitProxMax)
                        {
                            simpInitProxMax = triangle.second;
                        }
                    }

                    else
                    {
                        simpInitNonProxMean += triangle.second;
                        mean_non_prox_SI += triangle.second;
                        if (triangle.second > simpInitNonProxMax)
                        {
                            simpInitNonProxMax = triangle.second;
                        }
                    }
                }

                if (simpInitProxMean > 0)
                {
                    simpInitProxMean = simpInitProxMean / nbProxVerticesObj;
                }
                if (simpInitNonProxMean > 0)
                {
                    simpInitNonProxMean = simpInitNonProxMean / (m_initTriangleMeshes[i].m_vertices.size() - nbProxVerticesObj);
                }

                hausdorffDistances.second = {simpInitProxMean, simpInitProxMax, simpInitNonProxMean, simpInitNonProxMax};

                m_hausdorffDistances.push_back(hausdorffDistances);

                LOG(logINFO) << "Obj " << i << " Hausdorff distances computed.";
            }

            // writing Hausdorff distances in a file
            std::ofstream file("Hausdorff_distances.txt", std::ios::out | std::ios::trunc);
            CORE_ASSERT(file, "Error while opening Hausdorff distances file.");

            for (uint i = 0; i < m_meshContactElements.size(); i++)
            {
                file << "Mesh " << i << " :" << std::endl;
                file << "    Proximity mean : " << (m_hausdorffDistances[i].first[0] + m_hausdorffDistances[i].second[0]) / 2 << std::endl;
                file << "    Proximity max : " << std::max(m_hausdorffDistances[i].first[1], m_hausdorffDistances[i].second[1]) << std::endl;
                file << "    Non-proximity mean : " << (m_hausdorffDistances[i].first[2] + m_hausdorffDistances[i].second[2]) / 2 << std::endl;
                file << "    Non-proximity max : " << std::max(m_hausdorffDistances[i].first[3], m_hausdorffDistances[i].second[3]) << std::endl;
                file << " " << std::endl;

                if (max_prox < std::max(m_hausdorffDistances[i].first[1], m_hausdorffDistances[i].second[1]))
                {
                    max_prox = std::max(m_hausdorffDistances[i].first[1], m_hausdorffDistances[i].second[1]);
                }
                if (max_non_prox < std::max(m_hausdorffDistances[i].first[3], m_hausdorffDistances[i].second[3]))
                {
                    max_non_prox = std::max(m_hausdorffDistances[i].first[3], m_hausdorffDistances[i].second[3]);
                }
            }

            mean_prox_IS /= nbProxVertices_IS;
            mean_prox_SI /= nbProxVertices_SI;
            mean_prox = (mean_prox_IS + mean_prox_SI) / 2;
            file << "Proximity mean : " << mean_prox << std::endl;
            file << "Proximity max : " << max_prox << std::endl;
            mean_non_prox_IS /= nbVertices - nbProxVertices_IS;
            mean_non_prox_SI /= nbVertices - nbProxVertices_SI;
            mean_non_prox = (mean_non_prox_IS + mean_non_prox_SI) / 2;
            file << "Non-proximity mean : " << mean_non_prox << std::endl;
            file << "Non-proximity max : " << max_non_prox << std::endl;

            file.close();
        }

        void MeshContactManager::pipelineProx()
        {
//            setComputeR();
//            setComputeDistribution();
//            setComputeClusters();
//            setDisplayClusters();
            setDisplayWeight();
            setComputeAlpha();
            setDisplayProximities();
            setConstructM0();
            //setConstructM01();
            colorMeshesSimplified();
            HausdorffDistance();
        }

        void MeshContactManager::pipelineQEM()
        {
            setDisplayProximities();
            setConstructM0();
            //setConstructM01();
            //colorMeshesSimplified();
        }

        void MeshContactManager::normalize()
        {
            Ra::Core::VectorArray<Ra::Core::Vector3> v1 = m_meshContactElements[0]->getInitTriangleMesh().m_vertices;
            Super4PCS::AABB3D aabb1 = Super4PCS::AABB3D();
            for (uint a = 0; a < v1.size(); a++)
            {
                aabb1.extendTo(v1[a]);
            }
            Scalar height1 = aabb1.height();
            Scalar width1 = aabb1.width();
            Scalar depth1 = aabb1.depth();

            Ra::Core::VectorArray<Ra::Core::Vector3> v2 = m_meshContactElements[1]->getInitTriangleMesh().m_vertices;
            Super4PCS::AABB3D aabb2 = Super4PCS::AABB3D();
            for (uint a = 0; a < v2.size(); a++)
            {
                aabb2.extendTo(v2[a]);
            }
            Scalar height2 = aabb2.height();
            Scalar width2 = aabb2.width();
            Scalar depth2 = aabb2.depth();

            Scalar max = std::max(std::max(std::max(std::max(std::max(width2, depth2), height2), depth1), width1), height1);
            LOG(logINFO) << "Max : " << max << std::endl;

            std::string s;
            Scalar value;

            std::ifstream input("screw.obj", std::ios::in);
            CORE_ASSERT(input, "Error while opening obj file.");

            std::ofstream output("screw_normalized.obj", std::ios::out | std::ios::trunc);
            CORE_ASSERT(output, "Error while opening obj file.");

            Scalar newValue;

            while (!input.eof())
            {
                input >> s;
                if (s.compare("v") == 0)
                {
                    output << s;
                    for (uint i = 0; i < 3; i++)
                    {
                        input >> value;
                        newValue = value / max;
                        output << " " << newValue;
                    }
                    output << std::endl;
                }
                else
                {
                    output << s;
                    for (uint i = 0; i < 3; i++)
                    {
                        input >> s;
                        output << " " << s;
                    }
                    output << std::endl;
                }
            }

            output.close();
            input.close();
        }

        void MeshContactManager::scale(Scalar n)
        {
            std::string s;
            Scalar value;

            std::ifstream input("base_scale_2_proximity.obj", std::ios::in);
            CORE_ASSERT(input, "Error while opening obj file.");

            std::ofstream output("base_scale_1_proximity.obj", std::ios::out | std::ios::trunc);
            CORE_ASSERT(output, "Error while opening obj file.");

            Scalar newValue;

            while (!input.eof())
            {
                input >> s;
                if (s.compare("v") == 0)
                {
                    output << s;
                    for (uint i = 0; i < 3; i++)
                    {
                        input >> value;
                        newValue = value * n;
                        output << " " << newValue;
                    }
                    output << std::endl;
                }
                else
                {
                    output << s;
                    for (uint i = 0; i < 3; i++)
                    {
                        input >> s;
                        output << " " << s;
                    }
                    output << std::endl;
                }
            }

            output.close();
            input.close();
        }
    }
}
