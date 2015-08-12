#ifndef RADIUMENGINE_SPLINE_HPP__
#define RADIUMENGINE_SPLINE_HPP__

#include <vector>

namespace Ra
{
    namespace Core
    {
        namespace ESpline
        {
            enum Node_t 
            {
                UNIFORM,
                OPEN_UNIFORM ///< Connected to the first and last control points
            };
        }

        /**
         * @class Spline
         *
         * @brief Handling spline curves of arbitrary dimensions
         * @note This class use the efficient blossom algorithm to compute a position on
         * the curve.
         *
         * @tparam Point_t : type of a point operators such as '+' '*' must be correctly
         * overloaded. The default constructor must be defined to return the
         * null vector (0, 0, 0 ...)
         * @tparam Real_t ; floating point reprensentation of the points
         * (float, double etc.)
         */
        template<typename Point_t, typename Real_t>
        class Spline
        {
        public:
            /// Type of the nodal vector
            /// @param k : order of the spline (minimum is two)
            /// @param node_type : nodal vector type (uniform, open_uniform)
            /// This will define the behavior of the spline with its control points
            /// as well as its speed according to its parameter.
            Spline(int k = 2, ESpline::Node_t node_type = ESpline::OPEN_UNIFORM);

            /// Set the position of the spline control points.
            void set_ctrl_points(const std::vector<Point_t>& point);

            /// Get the control points of the spline
            void get_ctrl_points(std::vector<Point_t>& points) const;

            /// The the nodal vector type
            void set_node_type( ESpline::Node_t type);

            /// Evaluate position of the spline
            /// @param u : curve parameter ranging from [0; 1]
            Point_t eval_f(Real_t u) const;

            /// Evaluate speed of the spline
            Point_t eval_df(Real_t u) const;

            int get_order() const { return _k; }

        private:
            // -------------------------------------------------------------------------
            /// @name Class tools
            // -------------------------------------------------------------------------

            void assert_splines() const;

            /// set value and size of the nodal vector depending on the current number
            /// of control points
            void set_nodal_vector();

            /// Set values of the nodal vector to be uniform
            void set_node_to_uniform();

            /// Set values of the nodal vector to be open uniform
            void set_node_to_open_uniform();

            /// Evaluate the equation of a splines using the blossom algorithm
            /// @param u : the curve parameter which range from the values
            /// [node[k-1]; node[point.size()]]
            /// @param point : the control points which size must be at least equal to
            /// the order of the spline (point.size() >= k)
            /// @param k : the spline order (degree == k-1)
            /// @param node : the nodal vector which defines the speed of the spline
            /// parameter u. The nodal vector size must be equal to (k + point.size())
            /// @param off : offset to apply to the nodal vector 'node' before reading
            /// from it. this is useful to compute derivatives.
            Point_t eval(Real_t u,
                         const std::vector<Point_t>& point,
                         int k,
                         const std::vector<Real_t>& node,
                         int off = 0) const;

            Point_t eval_rec(Real_t u,
                             std::vector<Point_t> p_in,
                             int k,
                             std::vector<Real_t> node_in) const;

            // -------------------------------------------------------------------------
            /// @name attributes
            // -------------------------------------------------------------------------

            ESpline::Node_t _node_type;    ///< Nodal vector type
            int _k;                        ///< spline order
            std::vector<Point_t> _point;   ///< Control points
            std::vector<Point_t> _vec;     ///< Control points differences
            std::vector<Real_t>  _node;    ///< Nodal vector
        };
    }
}

#include "Spline.inl"

#endif
