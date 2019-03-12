#ifndef QUADRIC_H
#define QUADRIC_H

#include <Core/Math/Types.hpp>
#include <Core/RaCore.hpp>

#include <Eigen/Eigenvalues>

namespace Ra {
namespace Core {

/**
 * A representation for quadratic equation as:
 *     \f$ v^T A v + 2 b^T v + c \f$
 *
 * Reminder on ProgressiveMesh
 * ---------------------------
 *  - Quadric of the edge:
 *     \f$ Q_e = \sum Q_f \f$

 *    where \f$ f \f$ all the faces adjacent to \f$ e \f$ and
 *    \f$ Q_e = (A_e, b_e, c_e) \f$
 *  - Calculate ???: \f$ v_{min} = A_e^{-1} b_e \f$
 *  - Quadric error on the edge:
 *     \f$ err = v_{min}^T A_e v_{min} + 2 b_e^T v_{min} + c_e \f$
 */
template <int _DIM>
class Quadric {
  public:
    static constexpr int DIM = _DIM;
    using Matrix = Eigen::Matrix<Scalar, DIM, DIM>;
    using Vector = Eigen::Matrix<Scalar, DIM, 1>;

    /**
     * Construct an initialized quadric.
     */
    Quadric();

    /**
     * Construct and initialize a quadric.
     */
    Quadric( const Matrix& a, const Vector& b, double c ) : m_a( a ), m_b( b ), m_c( c ) {}

    /**
     * Construct a quadric from an other.
     */
    Quadric( const Quadric<DIM>& q );

    /**
     * Create a quadric from a normal vector n and the scalar which
     * represent the distance from the plane of normal n to the origin
     * \see compute
     */
    Quadric( const Vector& n, Scalar ndotp );

    ~Quadric();

    /// \name Component Access
    /// \{

    /**
     * Return the quadratic coefficient matrix.
     */
    inline const Matrix& getA() const;

    /**
     * Set the quadratic coefficient matrix.
     */
    inline void setA( const Matrix& a );

    /**
     * Return the linear coefficient vector.
     */
    inline const Vector& getB() const;

    /**
     * Set the linear coefficient vector.
     */
    inline void setB( const Vector& b );

    /**
     * Return the constant coefficient scalar.
     */
    inline const double& getC() const;

    /**
     * Set the constant coefficient scalar.
     */
    inline void setC( const double& c );
    /// \}

    /**
     * Create a quadric from a normal vector n and the scalar which
     * represent the distance from the plane of normal n to the origin.
     * \deprecated Use constructor instead.
     */
    [[deprecated]] void compute( const Vector& n, double ndotp );

    /**
     * Computes eigen values of matrix A.
     */
    inline typename Eigen::EigenSolver<Matrix3>::EigenvalueType computeEigenValuesA();

    /**
     * Computes eigen vectors of matrix A.
     */
    inline typename Eigen::EigenSolver<Matrix3>::EigenvectorsType computeEigenVectorsA();

    /// \name Arithmetics
    /// \{

    /**
     * Return the sum of *this and \p q.
     */
    inline Quadric operator+( const Quadric& q ) const;

    /**
     * Return the product of *this by \p scal.
     */
    inline Quadric operator*( const Scalar scal );

    /**
     * Return *this after performing the sum of *this and \p q.
     */
    inline Quadric& operator+=( const Quadric& q );

    /**
     * Return *this after performing the product of *this by \p scal.
     */
    inline Quadric& operator*=( Scalar scal );
    /// \}

  private:
    /// The quadratic coefficient matrix.
    Matrix m_a;

    /// The linear coefficient vector.
    Vector m_b;

    /// The constant coefficient scalar.
    double m_c;
};

} // namespace Core

} // namespace Ra

#include <Core/Math/Quadric.inl>

#endif // QUADRIC_H
