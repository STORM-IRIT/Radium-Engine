#ifndef RADIUMENGINE_POSITIONBASED_SKINNING_HPP
#define RADIUMENGINE_POSITIONBASED_SKINNING_HPP
#include <Core/Animation/Handle/HandleWeight.hpp>
#include <Core/Animation/Skinning/SkinningData.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Simulation/Simulation.h>

namespace PositionBasedSkinning {

class PBS {

  public:
    PBS();

    PBD::SimulationModel& getModel() { return m_model; }

    void setVolumetricMeshPath( const std::string& pathFile );

    bool isVolumetricMeshValid() const { return m_volumetricMeshValid; }

    void init( const Ra::Core::Vector3Array* surfaceVertices,
               const Ra::Core::Animation::Skeleton* skeleton );

    void compute( const Ra::Core::Animation::Skeleton* skeleton,
                  const Ra::Core::Vector3Array& intetMesh, const Ra::Core::Animation::Pose& pose,
                  const Ra::Core::Animation::WeightMatrix weight,
                  Ra::Core::Vector3Array& outtetMesh );

  private:
    // Mapping Volumetric Mesh - Surface Mesh
    typedef struct {
        unsigned int tetrahedronIndex;
        Ra::Core::Vector4 barycentricCoord;
    } LinkTetrahedron;

    std::vector<LinkTetrahedron> m_linksTetrahedronSurfaceVertex;

    //-------

    PBD::SimulationModel m_model;
    //
    bool m_volumetricMeshValid = false;

    const Ra::Core::Vector3Array* m_surfaceVertices;
    const Ra::Core::Animation::Skeleton* m_skeleton;

    Ra::Core::Animation::WeightMatrix m_weightParticles;

    // Step Functions
    Ra::Core::Vector3 m_gravity;
    unsigned int m_maxIter;

    //-------------------
    // Functions
    //---------------------

    /* Weight Particles */
    Scalar distancePointSegment( const Ra::Core::Vector3& point,
                                 const Ra::Core::Vector3& startSegment,
                                 const Ra::Core::Vector3& endSegment );
    void searchNearestBone( const Ra::Core::Vector3& particle, Ra::Core::Vector3& startBone,
                            Ra::Core::Vector3& endBone, unsigned int& indexNearestBone,
                            Scalar& distanceNearestBone );

    void smoothWeights( const Ra::Core::Graph::AdjacencyList& skeletonGraph,
                        const unsigned int indexParticle, const Ra::Core::Vector3& particle,
                        const unsigned int indexNearestBone, const Ra::Core::Vector3& startBone,
                        const Ra::Core::Vector3& endBone );

    void weightParticle( const Ra::Core::Graph::AdjacencyList& skeletonGraph,
                         const unsigned int indexParticle, const Ra::Core::Vector3& particle,
                         const unsigned int indexNearestBone, const Ra::Core::Vector3& startBone,
                         const Ra::Core::Vector3& endBone );

    void weightParticlesBindConstraint();

    /* Add Constraints */
    void addConstraint_stretch();
    void addConstraint_volume();

    /* Clear accelerations of dynamic particles */
    void clearAccelerations( PBD::ParticleData& pd );

    void positionConstraintProjection();

    /* Time Integration */
    void semiImplicitEuler( const Scalar h, PBD::ParticleData& pd );
    void velocityUpdateFirstOrder( const Scalar h, PBD::ParticleData& pd );

    void step( const Ra::Core::Animation::Skeleton* skeleton, PBD::ParticleData& pd );

    void linearBlendSkinning2TetMesh( const Ra::Core::Animation::Pose& pose,
                                      PBD::ParticleData& pd );
    /* Init Surface Mesh */
    Scalar computeDeterminant( const Ra::Core::Vector3& vertex0, const Ra::Core::Vector3& vertex1,
                               const Ra::Core::Vector3& vertex2, const Ra::Core::Vector3& vertex3 );

    void getDeterminants( const Ra::Core::Vector3& tetVertex0, const Ra::Core::Vector3& tetVertex1,
                          const Ra::Core::Vector3& tetVertex2, const Ra::Core::Vector3& tetVertex3,
                          const Ra::Core::Vector3& surfaceVertex, Scalar& determinant0,
                          Scalar& determinant1, Scalar& determinant2, Scalar& determinant3,
                          Scalar& determinant4 );

    const Ra::Core::Vector4 getBarycentricCoordinate( const Scalar determinant0,
                                                      const Scalar determinant1,
                                                      const Scalar determinant2,
                                                      const Scalar determinant3,
                                                      const Scalar determinant4 );

    bool checkPointInTetra( Scalar determinant0, Scalar determinant1, Scalar determinant2,
                            Scalar determinant3, Scalar determinant4 );

    // Calculate the distance between a point and a tetrahedron’s centroid
    Scalar getSquaredDistancePointToTetrahedron( const Ra::Core::Vector3& tetVertex0,
                                                 const Ra::Core::Vector3& tetVertex1,
                                                 const Ra::Core::Vector3& tetVertex2,
                                                 const Ra::Core::Vector3& tetVertex3,
                                                 const Ra::Core::Vector3& surfaceVertex );

    // Find the barycentric coordinate for a single vertex in a list of tetrahedra
    void findBarycentricCoordinateTetrahedronForVertex( const Ra::Core::Vector3& surfaceVertex,
                                                        const PBD::TetModel& tetras,
                                                        const PBD::ParticleData& pd );

    // Find the tetrahedral barycentric coordinates for the surface vertices list
    void findBarycentricCoordinatesTetrahedron();

    /* Update Surface Mesh */

    Ra::Core::Vector3 getPositionByBarycentricCoord( const PBD::TetModel& tetras,
                                                     const PBD::ParticleData& pd,
                                                     const LinkTetrahedron& linkTetra );
    void updateVertices( Ra::Core::Vector3Array& outMesh );

}; // PBS

} // namespace PositionBasedSkinning

#endif
