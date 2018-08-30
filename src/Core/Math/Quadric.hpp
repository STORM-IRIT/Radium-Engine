#ifndef QUADRIC_H
#define QUADRIC_H

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/RaCore.hpp>

#include <Eigen/Eigenvalues>

namespace Ra {
namespace Core {

/// A representation for quadratic equation as
///   \f$ v^T A v + 2 b^T v + c \f$
///
/// Reminder on ProgressiveMesh
/// ---------------------------
///
///     Calculate the Quadric of the edge Qe = sum(Qf) with f all the faces adjacent
///     to e
///
///     Calculate vmin = Ae^(-1) be                with Qe = (Ae, be, ce)
///
///     Calculate the error on the edge
///         error = vmin^T Ae vmin + 2 be^T vmin + ce       with Qe = (Ae, be, ce)

// template<int _DIM = 3>
template <int _DIM>
class Quadric {
  public:
    static constexpr int DIM = _DIM;
    using Matrix = Eigen::Matrix<Scalar, DIM, DIM>;
    using Vector = Eigen::Matrix<Scalar, DIM, 1>;

    Quadric();

    Quadric( const Matrix& a, const Vector& b, double c ) : m_a( a ), m_b( b ), m_c( c ) {}

    Quadric( const Quadric<DIM>& q );

    /// Create a quadric from a normal vector n and the scalar which
    /// represent the distance from the plane of normal n to the origin
    /// \see compute
    Quadric( const Vector& n, Scalar ndotp );

    ~Quadric();

    /// Return the quadratic coefficient matrix.
    inline const Matrix& getA() const;

    /// Set the quadratic coefficient matrix.
    inline void setA( const Matrix& a );

    /// Return the linear coefficient vector.
    inline const Vector& getB() const;

    /// Set the linear coefficient vector.
    inline void setB( const Vector& b );

    /// Return the constant coefficient scalar.
    inline const double& getC() const;

    /// Set the constant coefficient scalar.
    inline void setC( const double& c );

    /// Create a quadric from a normal vector n and the scalar which
    /// represent the distance from the plane of normal n to the origin
    /// \deprecated Use constructor instead
    void compute( const Vector& n, double ndotp );

    /// Computes eigen values of matrix A
    inline typename Eigen::EigenSolver<Matrix3>::EigenvalueType computeEigenValuesA();

    /// Computes eigen vvectors of matrix A
    inline typename Eigen::EigenSolver<Matrix3>::EigenvectorsType computeEigenVectorsA();

    /// Return the sum of *this and \p q.
    inline Quadric operator+( const Quadric& q ) const;

    /// Return the product of *this by \p scal.
    inline Quadric operator*( const Scalar scal );

    /// Return *this after performing the sum of *this and \p q.
    inline Quadric& operator+=( const Quadric& q );

    /// Return *this after performing the product of *this by \p scal.
    inline Quadric& operator*=( Scalar scal );

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
