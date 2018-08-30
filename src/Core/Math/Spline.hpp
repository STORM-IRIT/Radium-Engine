#ifndef RADIUMENGINE_SPLINE_HPP__
#define RADIUMENGINE_SPLINE_HPP__

#include <Core/Containers/VectorArray.hpp>
#include <Core/RaCore.hpp>
#include <vector>

namespace Ra {
namespace Core {

/**
 * @class Spline
 *
 * @brief Handling spline curves of arbitrary dimensions
 * @note This class use the efficient blossom algorithm to compute a position on
 * the curve.
 * @tparam D : dimension of the curve.
 * @tparam K : order of the curve (min 2)
 */
template <uint D, uint K = 2>
class Spline {
  public:
    /// Type of the nodal vector.
    /// This will define the behavior of the spline with its control points
    /// as well as its speed according to its parameter.
    enum Type {
        UNIFORM,
        OPEN_UNIFORM ///< Connected to the first and last control points
    };

    using Vector = typename Eigen::Matrix<Scalar, D, 1>;

  public:
    inline Spline( Type type = OPEN_UNIFORM );

    /// Set the position of the spline control points.
    inline void setCtrlPoints( const Core::VectorArray<Vector>& points );

    /// Return the control points of the spline.
    inline const Core::VectorArray<Vector>& getCtrlPoints() const;

    /// Return the nodal vector type.
    inline void setType( Type type );

    /// Return the point at the curvilinear parameter \p u.
    inline Vector f( Scalar u ) const;

    /// Return the tangent vector at the curvilinear parameter \p u.
    inline Vector df( Scalar u ) const;

  private:
    /// Assert the spline is correctly initialized.
    inline void assertSplines() const;

    /// Calls setNodeToUniform() or setNodeToOpenUniform() depending on the spline type.
    inline void setNodalVector();

    /// Initialize the nodal vector to be uniform, according to the number of control points.
    inline void setNodeToUniform();

    /// Initialize the nodal vector to be open uniform, according to the number of control points.
    inline void setNodeToOpenUniform();

    /// Evaluate the equation of a splines using the blossom algorithm.
    /// @param u : the curve parameter which range from the values
    /// [node[k-1]; node[point.size()]]
    /// @param point : the control points which size must be at least equal to
    /// the order of the spline (point.size() >= k)
    /// @param k : the spline order (degree == k-1)
    /// @param node : the nodal vector which defines the speed of the spline
    /// parameter u. The nodal vector size must be equal to (k + point.size())
    /// @param off : offset to apply to the nodal vector knots before using them.
    ///              This is useful when computing derivatives.
    /// @return the curve point at u.
    static inline Vector eval( Scalar u, const Core::VectorArray<Vector>& points,
                               const std::vector<Scalar>& node, uint k, int off = 0 );

    /// Internal recursive call for the blossom algorithm.
    static inline Vector evalRec( Scalar u, const Core::VectorArray<Vector>& points,
                                  const std::vector<Scalar>& node, uint k );

  private:
    /// Control points.
    Core::VectorArray<Vector> m_points;

    /// Control points differences.
    Core::VectorArray<Vector> m_vecs;

    /// Nodal vector.
    std::vector<Scalar> m_node;

    /// Nodal vector type.
    Type m_type;
};

} // namespace Core
} // namespace Ra

#include <Core/Math/Spline.inl>

#endif
