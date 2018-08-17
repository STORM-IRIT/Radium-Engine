#include "Constraints.h"
#include "SimulationModel.h"
#include "PositionBasedDynamics.h"
#include "IDFactory.h"

using namespace PBD;

int DistanceConstraint::TYPE_ID = IDFactory::getId();
int VolumeConstraint::TYPE_ID = IDFactory::getId();
int BindConstraint::TYPE_ID = IDFactory::getId();

//////////////////////////////////////////////////////////////////////////
// DistanceConstraint
//////////////////////////////////////////////////////////////////////////
bool DistanceConstraint::initConstraint(SimulationModel &model, const unsigned int particle1, const unsigned int particle2)
{
	m_bodies[0] = particle1;
	m_bodies[1] = particle2;
	ParticleData &pd = model.getParticles();

    const Ra::Core::Vector3 &x1_0 = pd.getPosition0(particle1);
    const Ra::Core::Vector3 &x2_0 = pd.getPosition0(particle2);

	m_restLength = (x2_0 - x1_0).norm();

	return true;
}

bool DistanceConstraint::solvePositionConstraint(SimulationModel &model)
{
	ParticleData &pd = model.getParticles();

	const unsigned i1 = m_bodies[0];
	const unsigned i2 = m_bodies[1];

    Ra::Core::Vector3 &x1 = pd.getPosition(i1);
    Ra::Core::Vector3 &x2 = pd.getPosition(i2);
    const Scalar invMass1 = pd.getInvMass(i1);
    const Scalar invMass2 = pd.getInvMass(i2);

    Ra::Core::Vector3 corr1, corr2;
	const bool res = PositionBasedDynamics::solve_DistanceConstraint(
		x1, invMass1, x2, invMass2,
        m_restLength, model.getCompressionStiffness(), model.getStretchStiffness(), corr1, corr2);

	if (res)
	{
		if (invMass1 != 0.0)
			x1 += corr1;
		if (invMass2 != 0.0)
			x2 += corr2;
	}
	return res;
}

//////////////////////////////////////////////////////////////////////////
// BindConstraint
//////////////////////////////////////////////////////////////////////////
bool BindConstraint::initConstraint(const unsigned int particle,
                                    const unsigned int indexNearestBone, const Scalar distanceClosestBone)
{
    *m_bodies = particle;
    m_restLength = distanceClosestBone;
    m_indexNearestBone = indexNearestBone;
    return true;
}

bool BindConstraint::solvePositionConstraint(SimulationModel &model)
{
    ParticleData &pd = model.getParticles();

    const unsigned indexParticle = *m_bodies;

    Ra::Core::Vector3 &positionParticle = pd.getPosition(indexParticle);
    const Scalar invMass = pd.getInvMass(indexParticle);

    Ra::Core::Vector3 startBone, endBone, corr;
    const Ra::Core::Animation::Skeleton *skeleton = model.getSkeleton();
    skeleton->getBonePoints(m_indexNearestBone, startBone, endBone);

    const bool res = PositionBasedDynamics::solve_BindConstraint(
        positionParticle, startBone, endBone, invMass,
        m_restLength, model.getCompressionStiffness(), model.getStretchStiffness(), corr);

    if (res)
    {
        if (invMass != 0.0)
            positionParticle += corr;
    }
    return res;
}

//////////////////////////////////////////////////////////////////////////
// VolumeConstraint
//////////////////////////////////////////////////////////////////////////

bool VolumeConstraint::initConstraint(SimulationModel &model, const unsigned int particle1, const unsigned int particle2,
	const unsigned int particle3, const unsigned int particle4)
{
	m_bodies[0] = particle1;
	m_bodies[1] = particle2;
	m_bodies[2] = particle3;
	m_bodies[3] = particle4;
	ParticleData &pd = model.getParticles();

    const Ra::Core::Vector3 &p0 = pd.getPosition0(particle1);
    const Ra::Core::Vector3 &p1 = pd.getPosition0(particle2);
    const Ra::Core::Vector3 &p2 = pd.getPosition0(particle3);
    const Ra::Core::Vector3 &p3 = pd.getPosition0(particle4);

	m_restVolume = fabs((1.0f / 6.0f) * (p3 - p0).dot((p2 - p0).cross(p1 - p0)));

	return true;
}

bool VolumeConstraint::solvePositionConstraint(SimulationModel &model)
{
	ParticleData &pd = model.getParticles();

	const unsigned i1 = m_bodies[0];
	const unsigned i2 = m_bodies[1];
	const unsigned i3 = m_bodies[2];
	const unsigned i4 = m_bodies[3];

    Ra::Core::Vector3 &x1 = pd.getPosition(i1);
    Ra::Core::Vector3 &x2 = pd.getPosition(i2);
    Ra::Core::Vector3 &x3 = pd.getPosition(i3);
    Ra::Core::Vector3 &x4 = pd.getPosition(i4);

    const Scalar invMass1 = pd.getInvMass(i1);
    const Scalar invMass2 = pd.getInvMass(i2);
    const Scalar invMass3 = pd.getInvMass(i3);
    const Scalar invMass4 = pd.getInvMass(i4);

    Ra::Core::Vector3 corr1, corr2, corr3, corr4;
	const bool res = PositionBasedDynamics::solve_VolumeConstraint(x1, invMass1,
		x2, invMass2,
		x3, invMass3,
		x4, invMass4,
		m_restVolume,
        model.getNegVolumeStiffness(),
        model.getPosVolumeStiffness(),
		corr1, corr2, corr3, corr4);

	if (res)
	{
		if (invMass1 != 0.0)
			x1 += corr1;
		if (invMass2 != 0.0)
			x2 += corr2;
		if (invMass3 != 0.0)
			x3 += corr3;
		if (invMass4 != 0.0)
			x4 += corr4;
	}
	return res;
}

