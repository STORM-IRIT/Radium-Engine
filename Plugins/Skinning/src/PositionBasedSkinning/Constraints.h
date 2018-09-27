#ifndef _CONSTRAINTS_H
#define _CONSTRAINTS_H

#include "ParticleData.h"
#include "Config.h"

namespace PBD
{
	class SimulationModel;

	class Constraint
	{
	public:
		unsigned int m_numberOfBodies;
		/** indices of the linked bodies */
		unsigned int *m_bodies;

		Constraint(const unsigned int numberOfBodies)
		{
			m_numberOfBodies = numberOfBodies;
			m_bodies = new unsigned int[numberOfBodies];
		}

        virtual ~Constraint() { delete[] m_bodies; }
        virtual int getTypeId() const = 0;

        virtual bool solvePositionConstraint(SimulationModel &model) { return true; }
	};

	class DistanceConstraint : public Constraint
	{
	public:
		static int TYPE_ID;
        Scalar m_restLength;

		DistanceConstraint() : Constraint(2) {}
        virtual int getTypeId() const { return TYPE_ID; }

		virtual bool initConstraint(SimulationModel &model, const unsigned int particle1, const unsigned int particle2);
		virtual bool solvePositionConstraint(SimulationModel &model);
	};

    class BindConstraint : public Constraint
    {
    public:
        static int TYPE_ID;
        Scalar m_restLength;
        unsigned int m_indexNearestBone;


        BindConstraint() : Constraint(1){}
        virtual int getTypeId() const { return TYPE_ID; }

        virtual bool initConstraint(const unsigned int particle, const unsigned int indexNearestBone, const Scalar distanceClosestBone);
        virtual bool solvePositionConstraint(SimulationModel &model);
    };

	class VolumeConstraint : public Constraint
	{
	public:
		static int TYPE_ID;
        Scalar m_restVolume;

		VolumeConstraint() : Constraint(4) {}
        virtual int getTypeId() const { return TYPE_ID; }

		virtual bool initConstraint(SimulationModel &model, const unsigned int particle1, const unsigned int particle2,
								const unsigned int particle3, const unsigned int particle4);
		virtual bool solvePositionConstraint(SimulationModel &model);
	};
}

#endif
