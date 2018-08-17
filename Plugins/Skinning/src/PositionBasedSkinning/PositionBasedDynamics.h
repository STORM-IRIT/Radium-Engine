#ifndef POSITION_BASED_DYNAMICS_H
#define POSITION_BASED_DYNAMICS_H

#include <Core/Math/LinearAlgebra.hpp>

// ------------------------------------------------------------------------------------
namespace PBD
{
	class PositionBasedDynamics
	{
	public:

		// -------------- standard PBD -----------------------------------------------------

		/** Determine the position corrections for a distance constraint between two particles:\n\n
		* \f$C(\mathbf{p}_0, \mathbf{p}_1) = \| \mathbf{p}_0 - \mathbf{p}_1\| - l_0 = 0\f$\n\n
		* More information can be found in the following papers: \cite Mueller07, \cite BMOT2013, \cite BMOTM2014, \cite BMM2015, 
		*
		* @param p0 position of first particle 
		* @param invMass0 inverse mass of first particle
		* @param p1 position of second particle
		* @param invMass1 inverse mass of second particle
		* @param restLength rest length of distance constraint
		* @param compressionStiffness stiffness coefficient for compression
		* @param stretchStiffness stiffness coefficient for stretching
		* @param corr0 position correction of first particle
		* @param corr1 position correction of second particle
		*/
		static bool solve_DistanceConstraint(
            const Ra::Core::Vector3 &p0, Scalar invMass0,
            const Ra::Core::Vector3 &p1, Scalar invMass1,
            const Scalar restLength,
            const Scalar compressionStiffness,
            const Scalar stretchStiffness,
            Ra::Core::Vector3 &corr0, Ra::Core::Vector3 &corr1);

        static bool solve_BindConstraint(
            const Ra::Core::Vector3 &particle,
            const Ra::Core::Vector3 &startBone, const Ra::Core::Vector3 &endBone,
            Scalar invMassParticle,
            const Scalar restLength,
            const Scalar compressionStiffness,
            const Scalar stretchStiffness,
            Ra::Core::Vector3 &corr);

		/** Determine the position corrections for a constraint that conserves the volume
		* of single tetrahedron. Such a constraint has the form
		* \f{equation*}{
		* C(\mathbf{p}_1, \mathbf{p}_2, \mathbf{p}_3, \mathbf{p}_4) = \frac{1}{6}
		* \left(\mathbf{p}_{2,1} \times \mathbf{p}_{3,1}\right) \cdot \mathbf{p}_{4,1} - V_0,
		* \f}
		* where \f$\mathbf{p}_1\f$, \f$\mathbf{p}_2\f$, \f$\mathbf{p}_3\f$ and \f$\mathbf{p}_4\f$ 
		* are the four corners of the tetrahedron and \f$V_0\f$ is its rest volume.\n\n
		* More information can be found in the following papers: \cite Mueller07, \cite BMOT2013, \cite BMOTM2014, \cite BMM2015,
		*
		* @param p0 position of first particle
		* @param invMass0 inverse mass of first particle
		* @param p1 position of second particle
		* @param invMass1 inverse mass of second particle
		* @param p2 position of third particle
		* @param invMass2 inverse mass of third particle
		* @param p3 position of fourth particle
		* @param invMass3 inverse mass of fourth particle
		* @param restVolume rest angle \f$V_0\f$
		* @param negVolumeStiffness stiffness coefficient for compression
		* @param posVolumeStiffness stiffness coefficient for stretching
		* @param corr0 position correction of first particle
		* @param corr1 position correction of second particle
		* @param corr2 position correction of third particle
		* @param corr3 position correction of fourth particle
		*/
		static bool solve_VolumeConstraint(
            const Ra::Core::Vector3 &p0, Scalar invMass0,
            const Ra::Core::Vector3 &p1, Scalar invMass1,
            const Ra::Core::Vector3 &p2, Scalar invMass2,
            const Ra::Core::Vector3 &p3, Scalar invMass3,
            const Scalar restVolume,
            const Scalar negVolumeStiffness,
            const Scalar posVolumeStiffness,
            Ra::Core::Vector3 &corr0, Ra::Core::Vector3 &corr1, Ra::Core::Vector3 &corr2, Ra::Core::Vector3 &corr3);

    private:
        static Ra::Core::Vector3 projectionPointSegment (const Ra::Core::Vector3 &point, const Ra::Core::Vector3 &startSegment, const Ra::Core::Vector3 &endSegment);

	};
}

#endif
