#ifndef __SIMULATIONMODEL_H__
#define __SIMULATIONMODEL_H__

#include "ParticleData.h"
#include "TetModel.h"
#include "libutils/Config.h"

#include <Core/Animation/Handle/Skeleton.hpp>

namespace PBD {
class Constraint;

class SimulationModel {
  public:
    SimulationModel();
    virtual ~SimulationModel();

    typedef std::vector<Constraint*> ConstraintVector;
    typedef std::vector<unsigned int> ConstraintGroup;
    typedef std::vector<ConstraintGroup> ConstraintGroupVector;

  private:
    const Ra::Core::Animation::Skeleton* m_skeleton;
    TetModel m_tetModel;
    ParticleData m_particles;
    ConstraintVector m_constraints;
    ConstraintGroupVector m_constraintGroups;

    Scalar m_stretchStiffness;
    Scalar m_compressionStiffness;
    Scalar m_negVolumeStiffness;
    Scalar m_posVolumeStiffness;

  public:
    /* Getters - Setters */
    bool setTetModel( const std::string& filename );

    ParticleData& getParticles() { return m_particles; }
    const ParticleData& getParticles() const { return m_particles; }
    TetModel* getTetModel() { return &m_tetModel; }
    ConstraintVector& getConstraints() { return m_constraints; }
    ConstraintGroupVector& getConstraintGroups() { return m_constraintGroups; }

    Scalar getStretchStiffness() const { return m_stretchStiffness; }
    void setStretchStiffness( const Scalar& stretchStiffness ) {
        m_stretchStiffness = stretchStiffness;
    }

    Scalar getCompressionStiffness() const { return m_compressionStiffness; }
    void setCompressionStiffness( const Scalar& compressionStiffness ) {
        m_compressionStiffness = compressionStiffness;
    }

    Scalar getNegVolumeStiffness() const { return m_negVolumeStiffness; }
    void setNegVolumeStiffness( const Scalar& negVolumeStiffness ) {
        m_negVolumeStiffness = negVolumeStiffness;
    }

    Scalar getPosVolumeStiffness() const { return m_posVolumeStiffness; }
    void setPosVolumeStiffness( const Scalar& posVolumeStiffness ) {
        m_posVolumeStiffness = posVolumeStiffness;
    }

    void setSkeleton( const Ra::Core::Animation::Skeleton* skeleton ) { m_skeleton = skeleton; }
    const Ra::Core::Animation::Skeleton* getSkeleton() { return m_skeleton; }

    //---------

    void cleanConstraints();

    void initConstraintGroups();

    bool addDistanceConstraint( const unsigned int particle1, const unsigned int particle2 );
    bool addVolumeConstraint( const unsigned int particle1, const unsigned int particle2,
                              const unsigned int particle3, const unsigned int particle4 );

    bool addBindConstraint( const unsigned int particle, const unsigned int indexNearestBone,
                            const Scalar distanceClosestBone );
};
} // namespace PBD

#endif
