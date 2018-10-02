#if defined(_WIN32) || defined(_WIN64)
#define OS_WINDOWS
#define  __FLT_MAX__   FLT_MAX
#endif

#include <QApplication>
#include <QMessageBox>

#include "PBS.hpp"
#include <float.h>
#include "TimeManager.h"
#include "Constraints.h"

#include "../src/Engine/Managers/SystemDisplay/SystemDisplay.hpp"

//------------------------------------------------------------------

using namespace PBD;
using namespace PositionBasedSkinning;
using namespace std;
using namespace Ra::Core;

//------------------------------------------------------------------

PBS::PBS() :
    m_volumetricMeshValid{false},
    m_gravity{0.0, -9.81, 0.0},
    m_maxIter{12}
{

}

void PBS::init(const Vector3Array *surfaceVertices, const Animation::Skeleton *skeleton)
{
    m_surfaceVertices = surfaceVertices;
    m_skeleton = skeleton;
}

void PBS::setVolumetricMeshPath (const std::string &pathFile)
{
    m_volumetricMeshValid = m_model.setTetModel(pathFile);

    if (m_volumetricMeshValid)
    {
        findBarycentricCoordinatesTetrahedron();

        addConstraint_volume();
        weightParticlesBindConstraint();
        addConstraint_stretch();
        m_model.initConstraintGroups();
    }
}

/* -----------------------------------
  Weight Particles Bind Constraint
 ------------------------------------- */

Scalar PBS::distancePointSegment (const Vector3 &point, const Vector3 &startSegment, const Vector3 &endSegment)
{
    Scalar distance;
    Vector3 segment = endSegment - startSegment;
    Vector3 startSegmentPoint = point - startSegment;

    const Scalar normSegment = segment.squaredNorm();
    if (normSegment == 0.0)
    {
        distance = startSegmentPoint.norm();
    }
    else
    {
        Scalar dotSegmentPoint = startSegmentPoint.dot(segment);
        Scalar t = dotSegmentPoint / normSegment;
        t = std::max(0.0f, std::min(1.0f, t));
        const Vector3 projection = startSegment + t * segment;
        const Vector3 pointProjection = projection - point;
        distance = pointProjection.norm();
    }

    return distance;
}

void PBS::smoothWeights (const Graph::AdjacencyList &skeletonGraph, const unsigned int indexParticle,
                         const Vector3 &particle, const unsigned int indexNearestBone,
                         const Vector3 &startBone, const Vector3 &endBone)
{
    // Determine Closest end points
    Scalar distanceStartBone = (startBone - particle).squaredNorm();
    Scalar distanceEndBone = (endBone - particle).squaredNorm();

    if ( distanceStartBone < distanceEndBone )
    {
        if ( !skeletonGraph.isRoot(indexNearestBone) )
        {
            unsigned int parentIndex = skeletonGraph.m_parent.at(indexNearestBone);
            m_weightParticles.coeffRef(indexParticle, parentIndex) = 0.5;
            m_weightParticles.coeffRef(indexParticle, indexNearestBone) = 0.5;
        }
        else
        {
            m_weightParticles.coeffRef(indexParticle, indexNearestBone) = 1.0;
        }
    }
    else
    {
        const unsigned int num_jointsBone = skeletonGraph.m_child.at(indexNearestBone).size() + 1;
        const Scalar weight = 1.0 / num_jointsBone;

        m_weightParticles.coeffRef(indexParticle, indexNearestBone) = weight;

        auto indicesChildrenNearestBone = skeletonGraph.m_child[indexNearestBone];
        for( auto it = indicesChildrenNearestBone.cbegin(); it != indicesChildrenNearestBone.cend(); ++it )
        {
            m_weightParticles.coeffRef(indexParticle, *it) = weight;
        }
    }
}

void PBS::weightParticle (const Graph::AdjacencyList &skeletonGraph, const unsigned int indexParticle, const Vector3 &particle,
                          const unsigned int indexNearestBone, const Vector3 &startBone, const Vector3 &endBone)
{
    Vector3 startBoneParticle = particle - startBone;
    Vector3 bone = endBone - startBone;
    Scalar normBone = bone.norm();

    startBoneParticle /= normBone;
    bone.normalize();

    Scalar factor = startBoneParticle.dot(bone);

    if ( (factor >= (1.0 / 6.0)) && (factor <= (5.0 / 6.0)) )
    {
        m_weightParticles.coeffRef(indexParticle, indexNearestBone) = 1.0;
    }
    else
    {
        smoothWeights(skeletonGraph, indexParticle, particle, indexNearestBone, startBone, endBone);
    }
}

void PBS::searchNearestBone (const  Vector3 &particle, Vector3 &startBone, Vector3 &endBone,
                             unsigned int &indexNearestBone, Scalar &distanceNearestBone)
{
    Vector3 aStartBone, aEndBone;
    const unsigned int num_bones = m_skeleton->size();

    indexNearestBone = 0;
    m_skeleton->getBonePoints(indexNearestBone, startBone, endBone);
    distanceNearestBone = distancePointSegment(particle, startBone, endBone);

    for (auto i = 1; i < num_bones; ++i)
    {
        m_skeleton->getBonePoints(i, aStartBone, aEndBone);
        Scalar distanceBone = distancePointSegment(particle, aStartBone, aEndBone);
        if (distanceBone < distanceNearestBone)
        {
            indexNearestBone = i;
            distanceNearestBone = distanceBone;
            startBone = aStartBone;
            endBone = aEndBone;
        }
    }
}

void PBS::weightParticlesBindConstraint ()
{
    unsigned int indexNearestBone;
    Scalar distanceNearestBone;
    Vector3 startBone, endBone;
    ParticleData &pd = m_model.getParticles();
    const unsigned int num_particles = pd.size();

    m_weightParticles.resize(num_particles, m_skeleton->size());
    m_weightParticles.setZero();

    for (auto i = 0; i < num_particles; ++i)
    {
        Vector3 particle_position = pd.getPosition(i);
        searchNearestBone(particle_position, startBone, endBone, indexNearestBone, distanceNearestBone);

        // Attribute Weight
        weightParticle(m_skeleton->m_graph, i, particle_position, indexNearestBone, startBone, endBone);

        // Add Bind Constraint
        m_model.addBindConstraint(i, indexNearestBone, distanceNearestBone);
    }
}

/* ----------------------
  Add Constraints
 ----------------------- */

void PBS::addConstraint_stretch ()
{
    unsigned int endPoint1, endPoint2;
    TetModel *tetrahedra = m_model.getTetModel();
    TetModel::Edges set_edges = tetrahedra->getEdges();

    // Parallelisme ?
    for (auto it_edges = set_edges.cbegin(); it_edges != set_edges.cend(); ++it_edges)
    {
       endPoint1 = it_edges->m_vert[0];
       endPoint2 = it_edges->m_vert[1];
       m_model.addDistanceConstraint(endPoint1, endPoint2);
    }
}

void PBS::addConstraint_volume ()
{
    TetModel *tetrahedra = m_model.getTetModel();
    TetModel::Tets tets = tetrahedra->getTets();

    for (auto it_tet = tets.cbegin(); it_tet != tets.cend(); it_tet += 4)
    {
        m_model.addVolumeConstraint(*it_tet, *(it_tet + 1), *(it_tet + 2), *(it_tet + 3));
    }
}


//-----------------------------------------------------------------------------------------------------------

void PBS::compute ( const Animation::Skeleton *skeleton, const Animation::Pose &pose,
                     Ra::Core::Vector3Array &outMesh)
{
    if (m_volumetricMeshValid)
    {
        ParticleData &pd = m_model.getParticles();
         linearBlendSkinning(pose, pd);
         step(skeleton, pd);
         updateVertices(outMesh);

        /*for (auto i = 0; i < pd.size(); ++i)
        {
                 RA_DISPLAY_POINT( pd.getPosition(i), Ra::Core::Colors::Red(), 0.1f );
        }*/
    }
    else
    {

        QMessageBox messageBox;
        messageBox.critical(0,"Tetrahedral Mesh"," Tetrahedral mesh must be loaded first!");
        messageBox.setFixedSize(500,200);
    }

}

void PBS::linearBlendSkinning(const Animation::Pose &pose, ParticleData &pd)
{
    pd.resetCurrentPositions();

    for( int k = 0; k < m_weightParticles.outerSize(); ++k )
    {
        const int nonZero = m_weightParticles.col( k ).nonZeros();
        Animation::WeightMatrix::InnerIterator it0( m_weightParticles, k );
        #pragma omp parallel for
        for( int nz = 0; nz < nonZero; ++nz )
        {
            Animation::WeightMatrix::InnerIterator it = it0 + Eigen::Index(nz);
            const uint   i = it.row();
            const uint   j = it.col();
            const Scalar w = it.value();
            pd.getPosition(i) += w * ( pose[j] * pd.getPosition0(i) );
        }
    }
}

void PBS::clearAccelerations(ParticleData &pd)
{
    const unsigned int num_particles = pd.size();

    for (auto i = 0; i < num_particles; i++)
    {
        if (pd.getMass(i) != 0.0)
        {

            pd.setAcceleration( i, m_gravity );

        }
    }
}

void PBS::positionConstraintProjection()
{
    unsigned int iter = 0;

    SimulationModel::ConstraintVector &constraints = m_model.getConstraints();
    SimulationModel::ConstraintGroupVector const &groups = m_model.getConstraintGroups();

    while (iter < m_maxIter)
    {
        for (unsigned int group = 0; group < groups.size(); group++)
        {
            const int groupSize = (int)groups[group].size();
            #pragma omp parallel if(groupSize > MIN_PARALLEL_SIZE) default(shared)
            {
                #pragma omp for schedule(static)
                for (int i = 0; i < groupSize; i++)
                {
                    const unsigned int constraintIndex = groups[group][i];
                    constraints[constraintIndex]->solvePositionConstraint(m_model);
                }
            }
        }

        iter++;
    }
}

/* ----------------------
  Time Integration
 ----------------------- */

void PBS::semiImplicitEuler(const Scalar h, ParticleData &pd)
{
    const unsigned int num_particles = pd.size();

    for (auto i = 0; i < num_particles; i++)
    {
        if (pd.getMass(i) != 0.0)
        {
            pd.getVelocity(i) += pd.getAcceleration(i) * h;
            pd.getPosition(i) += pd.getVelocity(i) * h;
        }
    }
}

void PBS::velocityUpdateFirstOrder(const Scalar h, ParticleData &pd)
{
    const unsigned int num_particles = pd.size();

    for (auto i = 0; i < num_particles; i++)
    {
        if (pd.getMass(i) != 0.0)
            pd.getVelocity(i) = (1.0f / h) * (pd.getPosition(i) - pd.getOldPosition(i));
    }
}

//-----------------------------------------------------------------------------------------------------------

void PBS::step(const Animation::Skeleton *skeleton, ParticleData &pd)
{
    TimeManager *tm = TimeManager::getCurrent ();
    const Scalar h = tm->getTimeStepSize();

    m_model.setSkeleton(skeleton);
    clearAccelerations(pd);

    semiImplicitEuler(h, pd);
    positionConstraintProjection();
    velocityUpdateFirstOrder(h, pd);

    // compute new time
    tm->setTime (tm->getTime () + h);
}

//----------------------------
// Link to Surface Mesh
//---------------------------------

Scalar PBS::computeDeterminant( const Vector3 &vertex0, const Vector3 &vertex1,
                            const Vector3 &vertex2, const Vector3 &vertex3 )
{
   return (1.0f / 6.0f) * (vertex3 - vertex0).dot((vertex2 - vertex0).cross(vertex1 - vertex0));
}

void PBS::getDeterminants ( const Vector3 &tetVertex0, const Vector3 &tetVertex1,
                       const Vector3 &tetVertex2, const Vector3 &tetVertex3, const Vector3 &surfaceVertex,
                       Scalar &determinant0, Scalar &determinant1, Scalar &determinant2, Scalar &determinant3, Scalar &determinant4)
{
    determinant0 = computeDeterminant(tetVertex0, tetVertex1, tetVertex2, tetVertex3);
    determinant1 = computeDeterminant(surfaceVertex, tetVertex1, tetVertex2, tetVertex3);
    determinant2 = computeDeterminant(tetVertex0, surfaceVertex, tetVertex2, tetVertex3);
    determinant3 = computeDeterminant(tetVertex0, tetVertex1, surfaceVertex, tetVertex3);
    determinant4 = computeDeterminant(tetVertex0, tetVertex1, tetVertex2, surfaceVertex);
}

//Barycentric coordinate for a single vertex and a single tetrahedron
const Vector4 PBS::getBarycentricCoordinate(const Scalar determinant0, const Scalar determinant1,
                                             const Scalar determinant2, const Scalar determinant3, const Scalar determinant4)
{
    Vector4 barycentricCoord;

    barycentricCoord[0] = (determinant1 / determinant0);
    barycentricCoord[1] = (determinant2 / determinant0);
    barycentricCoord[2] = (determinant3 / determinant0);
    barycentricCoord[3] = (determinant4 / determinant0);

    return barycentricCoord;
}

/*
Checks whether the specified point is inside the tetrahedron
If by chance the Determinant det0 is 0, then your tetrahedron is degenerate (the points are coplanar).
If any other Di=0, then P lies on boundary i (boundary i being that boundary formed by the three points other than Vi).
If the sign of any Di differs from that of D0 then P is outside boundary i.
If the sign of any Di equals that of D0 then P is inside boundary i.
If P is inside all 4 boundaries, then it is inside the tetrahedron.
As a check, it must be that D0 = D1+D2+D3+D4.
The pattern here should be clear; the computations can be extended to simplicies of any dimension. (The 2D and 3D case are the triangle and the tetrahedron).
If it is meaningful to you, the quantities bi = Di/D0 are the usual barycentric coordinates.
Comparing signs of Di and D0 is only a check that P and Vi are on the same side of boundary i.
*/
bool PBS::checkPointInTetra(Scalar determinant0, Scalar determinant1,
                                  Scalar determinant2, Scalar determinant3, Scalar determinant4)
{
    bool insideTetra = (determinant0 != 0);
    insideTetra &= ( (determinant0 < 0) && ( ((determinant1 < 0) && (determinant2 < 0) && (determinant3 < 0) && (determinant4 < 0)) ) )
            || ( (determinant0 > 0) && ( (determinant1 > 0) && (determinant2 > 0) && (determinant3 > 0) && (determinant4 > 0) ) );
    return insideTetra;
}

Scalar PBS::getSquaredDistancePointToTetrahedron( const Vector3 &tetVertex0, const Vector3 &tetVertex1,
                                                  const Vector3 &tetVertex2, const Vector3 &tetVertex3, const Vector3 &surfaceVertex)
{
    const Vector3 tetrahedronCentroid = (tetVertex0 + tetVertex1 + tetVertex2 + tetVertex3) * 0.25;
    const Vector3 particleCentroid = tetrahedronCentroid - surfaceVertex;
    return particleCentroid.squaredNorm();
}


/*
 * If the point is not inside the tetrahedron
 * we have to find the tetrahedron whose centroid has the smallest distance to our point P.
*/
void PBS::findBarycentricCoordinateTetrahedronForVertex(const Vector3 &surfaceVertex, const TetModel::Tets &tetras, const ParticleData &pd)
{
    Scalar determinant0, determinant1, determinant2, determinant3, determinant4;
    Scalar minDist = __FLT_MAX__;
    LinkTetrahedron closestTetrahedron;
    bool foundInsideATetra{false};
    const unsigned int num_tetras = tetras.size();

    for (unsigned int i = 0 ; i < num_tetras && !foundInsideATetra ; i+=4)
    {
        getDeterminants(pd.getPosition(tetras[i]), pd.getPosition(tetras[i + 1]), pd.getPosition(tetras[i + 2]), pd.getPosition(tetras[i + 3]), surfaceVertex,
                determinant0, determinant1, determinant2, determinant3, determinant4);

        foundInsideATetra = checkPointInTetra(determinant0, determinant1, determinant2, determinant3, determinant4);
        Vector4 barycentricCoordonates = getBarycentricCoordinate(determinant0, determinant1, determinant2, determinant3, determinant4);

        if ( foundInsideATetra )
        {
            m_linksTetrahedronSurfaceVertex.push_back({i, barycentricCoordonates});
        }
        else
        {
            Scalar dist = getSquaredDistancePointToTetrahedron(pd.getPosition(tetras[i]), pd.getPosition(tetras[i + 1]),
                    pd.getPosition(tetras[i + 2]), pd.getPosition(tetras[i + 3]), surfaceVertex);

            if (dist < minDist)
            {
                minDist = dist;
                closestTetrahedron.tetrahedronIndex = i;
                closestTetrahedron.barycentricCoord = barycentricCoordonates;
            }
        }
    }

    if (!foundInsideATetra)
        m_linksTetrahedronSurfaceVertex.push_back(closestTetrahedron);
}

void PBS::findBarycentricCoordinatesTetrahedron()
{
    const unsigned int num_surfaceVertices = m_surfaceVertices->size();
    ParticleData &pd = m_model.getParticles();
    TetModel *tetrahedra = m_model.getTetModel();
    const TetModel::Tets tetras = tetrahedra->getTets();

    m_linksTetrahedronSurfaceVertex.clear();
    m_linksTetrahedronSurfaceVertex.reserve(num_surfaceVertices);

    for (auto i = 0; i < num_surfaceVertices ; ++i)
    {
        findBarycentricCoordinateTetrahedronForVertex(m_surfaceVertices->at(i), tetras, pd);
    }
}

//----------------------------
// Update Surface Mesh
//---------------------------------

Vector3 PBS::getPositionByBarycentricCoord(const TetModel::Tets &tetras, const ParticleData &pd, const LinkTetrahedron &linkTetra)
{
    Vector3 pos = pd.getPosition(tetras[linkTetra.tetrahedronIndex]) * linkTetra.barycentricCoord[0];
    for (auto i = 1; i < 4; ++i)
    {
        const auto tetraIndex = linkTetra.tetrahedronIndex + i;
        pos += pd.getPosition(tetras[tetraIndex]) * linkTetra.barycentricCoord[i];
    }
    return pos;
}

void PBS::updateVertices( Ra::Core::Vector3Array &outMesh)
{
    TetModel *tetrahedra = m_model.getTetModel();
    const TetModel::Tets tetras = tetrahedra->getTets();
    const ParticleData &pd = m_model.getParticles();
    const unsigned int num_surfaceVertices = outMesh.size();

    //#pragma omp parallel for
    for (auto i = 0; i < num_surfaceVertices; ++i)
    {
      outMesh[i] = getPositionByBarycentricCoord(tetras, pd, m_linksTetrahedronSurfaceVertex[i]);
    }
}
