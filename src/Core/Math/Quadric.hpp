#pragma once

#include <Core/RaCore.hpp>
#include <Core/Types.hpp>

#include <Eigen/Eigenvalues>

namespace Ra {
namespace Core {

/// A representation for quadratic equation as
/// v^T A v + 2 b^T v + c
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
class Quadric
{
  public:
    static constexpr int DIM = _DIM;
    using Matrix             = Eigen::Matrix<Scalar, DIM, DIM>;
    using Vector             = Eigen::Matrix<Scalar, DIM, 1>;

    /// Construct an initialized quadric.
    Quadric();

    /// Construct and initialize a quadric.
    Quadric( const Matrix& a, const Vector& b, double c ) : m_a( a ), m_b( b ), m_c( c ) {}

    /// Construct a quadric from an other.
    Quadric( const Quadric<DIM>& q );

    /// Create a quadric from a normal vector n and the scalar which
    /// represent the distance from the plane of normal n to the origin
    /// \see compute
    Quadric( const Vector& n, Scalar ndotp );

    ~Quadric();

    /// Getters and setters

    inline const Matrix& getA() const;
    inline void setA( const Matrix& a );

    inline const Vector& getB() const;
    inline void setB( const Vector& b );

    inline const double& getC() const;
    inline void setC( const double& c );

    /// Create a quadric from a normal vector n and the scalar which
    /// represent the distance from the plane of normal n to the origin
    /// \deprecated Use constructor instead
    [[deprecated]] void compute( const Vector& n, double ndotp );

    /// Computes eigen values and vectors of matrix A
    inline typename Eigen::EigenSolver<Matrix3>::EigenvalueType computeEigenValuesA();
    inline typename Eigen::EigenSolver<Matrix3>::EigenvectorsType computeEigenVectorsA();

    /// Operators

    inline Quadric operator+( const Quadric& q ) const;
    inline Quadric operator*( const Scalar scal );

    inline Quadric& operator+=( const Quadric& q );
    inline Quadric& operator*=( Scalar scal );

  private:
    Matrix m_a;
    Vector m_b;
    double m_c;
};

template <int DIM>
inline Quadric<DIM>::Quadric() : m_a( Matrix::Zero() ), m_b( Vector::Zero() ), m_c( 0 ) {}

template <int DIM>
inline Quadric<DIM>::Quadric( const Quadric<DIM>& q ) : m_a( q.m_a ), m_b( q.m_b ), m_c( q.m_c ) {}

template <int DIM>
inline Quadric<DIM>::Quadric( const Vector& n, Scalar ndotp ) :
    m_a( n * n.transpose() ), m_b( ndotp * n ), m_c( ndotp * ndotp ) {}

template <int DIM>
inline Quadric<DIM>::~Quadric() {}

template <int DIM>
inline void Quadric<DIM>::compute( const Vector& n, double ndotp ) {
    m_a = n * n.transpose();
    m_b = ndotp * n;
    m_c = ndotp * ndotp;
}

template <int DIM>
inline const typename Quadric<DIM>::Matrix& Quadric<DIM>::getA() const {
    return m_a;
}

template <int DIM>
inline void Quadric<DIM>::setA( const Matrix& a ) {
    m_a = a;
}

template <int DIM>
inline const typename Quadric<DIM>::Vector& Quadric<DIM>::getB() const {
    return m_b;
}

template <int DIM>
inline void Quadric<DIM>::setB( const Vector& b ) {
    m_b = b;
}

template <int DIM>
inline const double& Quadric<DIM>::getC() const {
    return m_c;
}

template <int DIM>
inline void Quadric<DIM>::setC( const double& c ) {
    m_c = c;
}

template <int DIM>
inline typename Eigen::EigenSolver<Matrix3>::EigenvalueType Quadric<DIM>::computeEigenValuesA() {
    typename Eigen::EigenSolver<Matrix3> es( m_a );
    return es.eigenvalues();
}

template <int DIM>
inline typename Eigen::EigenSolver<Matrix3>::EigenvectorsType Quadric<DIM>::computeEigenVectorsA() {
    typename Eigen::EigenSolver<Matrix3> es( m_a );
    return es.eigenvectors();
}

template <int DIM>
inline Quadric<DIM> Quadric<DIM>::operator+( const Quadric& q ) const {
    return Quadric<DIM>( m_a + q.getA(), m_b + q.getB(), m_c + q.getC() );
}

template <int DIM>
inline Quadric<DIM> Quadric<DIM>::operator*( const Scalar scal ) {
    return Quadric<DIM>( m_a * scal, m_b * scal, m_c * scal );
}

template <int DIM>
inline Quadric<DIM>& Quadric<DIM>::operator+=( const Quadric& q ) {
    m_a += q.getA();
    m_b += q.getB();
    m_c += q.getC();
    return *this;
}

template <int DIM>
inline Quadric<DIM>& Quadric<DIM>::operator*=( Scalar scal ) {
    m_a *= scal;
    m_b *= scal;
    m_c *= scal;
    return *this;
}

} // namespace Core
} // namespace Ra
