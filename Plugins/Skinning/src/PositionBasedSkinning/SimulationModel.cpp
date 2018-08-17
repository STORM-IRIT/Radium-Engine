#include "SimulationModel.h"
#include "Constraints.h"

using namespace PBD;

SimulationModel::SimulationModel() :
    m_stretchStiffness{1.0f},
    m_compressionStiffness{1.0f},
    m_negVolumeStiffness{1.0f},
    m_posVolumeStiffness{1.0f}
{

}

SimulationModel::~SimulationModel(void)
{
    cleanConstraints();
    m_particles.release();
}

void SimulationModel::cleanConstraints()
{
    for (unsigned int i = 0; i < m_constraints.size(); i++)
    {
        delete m_constraints[i];
    }
    m_constraints.clear();
}

bool SimulationModel::addDistanceConstraint(const unsigned int particle1, const unsigned int particle2)
{
	DistanceConstraint *c = new DistanceConstraint();
	const bool res = c->initConstraint(*this, particle1, particle2);
	if (res)
	{
		m_constraints.push_back(c);
	}
	return res;
}

bool SimulationModel::addVolumeConstraint(const unsigned int particle1, const unsigned int particle2,
										const unsigned int particle3, const unsigned int particle4)
{
	VolumeConstraint *c = new VolumeConstraint();
	const bool res = c->initConstraint(*this, particle1, particle2, particle3, particle4);
	if (res)
	{
		m_constraints.push_back(c);
	}
	return res;
}

bool SimulationModel::addBindConstraint (const unsigned int particle,
                                         const unsigned int indexNearestBone, const Scalar distanceClosestBone) {
    BindConstraint *c = new BindConstraint();
    const bool res = c->initConstraint(particle, indexNearestBone, distanceClosestBone);
    if (res)
    {
        m_constraints.push_back(c);
    }
    return res;
}

bool SimulationModel::setTetModel(const std::string &filename)
{
    bool success = m_tetModel.initMesh(filename, m_particles);
    if (success)
    {
        cleanConstraints();
    }
    return success;
}

void SimulationModel::initConstraintGroups()
{
	const unsigned int numConstraints = (unsigned int) m_constraints.size();
	const unsigned int numParticles = (unsigned int) m_particles.size();
	m_constraintGroups.clear();

	// Maps in which group a particle is or 0 if not yet mapped
	std::vector<unsigned char*> mapping;

	for (unsigned int i = 0; i < numConstraints; i++)
	{
		Constraint *constraint = m_constraints[i];

		bool addToNewGroup = true;
		for (unsigned int j = 0; j < m_constraintGroups.size(); j++)
		{
			bool addToThisGroup = true;

			for (unsigned int k = 0; k < constraint->m_numberOfBodies; k++)
			{
				if (mapping[j][constraint->m_bodies[k]] != 0)
				{
					addToThisGroup = false;
					break;
				}
			}

			if (addToThisGroup)
			{
				m_constraintGroups[j].push_back(i);

				for (unsigned int k = 0; k < constraint->m_numberOfBodies; k++)
                {
					mapping[j][constraint->m_bodies[k]] = 1;
                }
				addToNewGroup = false;
				break;
			}
		}
		if (addToNewGroup)
		{
            mapping.push_back(new unsigned char[numParticles]);
            memset(mapping[mapping.size() - 1], 0, sizeof(unsigned char)*numParticles);
			m_constraintGroups.resize(m_constraintGroups.size() + 1);
			m_constraintGroups[m_constraintGroups.size()-1].push_back(i);
			for (unsigned int k = 0; k < constraint->m_numberOfBodies; k++)
            {
				mapping[m_constraintGroups.size() - 1][constraint->m_bodies[k]] = 1;
            }
		}
	}

	for (unsigned int i = 0; i < mapping.size(); i++)
	{
		delete[] mapping[i];
	}
	mapping.clear();
}
