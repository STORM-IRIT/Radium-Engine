#include "Core/Math/Spline.hpp"

#include <cassert>
#include <algorithm>

namespace Ra
{
    namespace Core
    {
        template<typename Point_t, typename Real_t>
        Spline<Point_t, Real_t>::Spline(
                int k,
                ESpline::Node_t node_type)
            :
              _node_type(node_type),
              _k(k),
              _point( _k ),
              _vec( _k-1 ),
              _node( _k + _point.size() )
        {
            assert_splines();
        }

        // -----------------------------------------------------------------------------

        template<typename Point_t, typename Real_t>
        void Spline<Point_t, Real_t>::set_ctrl_points(const std::vector<Point_t>& point)
        {
            _point = point;
            _vec.resize(_point.size() - 1);
            for(int i = 0; i < (int)_vec.size(); ++i)
                _vec[i] = _point[i + 1] - _point[i];
            set_nodal_vector();
            assert_splines();

            for(int i = 0; i < (int)_vec.size(); ++i)
                _vec[i] /= _node[_k+i] - _node[i+1];
        }

        // -----------------------------------------------------------------------------

        template<typename Point_t, typename Real_t>
        void Spline<Point_t, Real_t>::get_ctrl_points(std::vector<Point_t>& points) const
        {
            points = _point;
        }

        // -----------------------------------------------------------------------------

        /// The the nodal vector type
        template<typename Point_t, typename Real_t>
        void Spline<Point_t, Real_t>::set_node_type( ESpline::Node_t type)
        {
            _node_type = type;
            set_nodal_vector();
            assert_splines();
        }

        // -----------------------------------------------------------------------------

        template<typename Point_t, typename Real_t>
        Point_t Spline<Point_t, Real_t>::eval_f(Real_t u) const
        {
            u = std::max(std::min(u, (Real_t)1), (Real_t)0); // clamp between [0 1]
            return eval(u, _point, _k, _node);
        }

        // -----------------------------------------------------------------------------

        template<typename Point_t, typename Real_t>
        Point_t Spline<Point_t, Real_t>::eval_df(Real_t u) const
        {
            u = std::max(std::min(u, (Real_t)1), (Real_t)0); // clamp between [0 1]
            return eval(u, _vec, (_k-1), _node, 1) * (Real_t)(_k-1);
        }

        // -----------------------------------------------------------------------------

        template<typename Point_t, typename Real_t>
        void Spline<Point_t, Real_t>::assert_splines() const
        {
            assert( _k > 1);
            assert((int)_point.size() >= _k );
            assert(_node. size() == (_k + _point.size()) );
            assert(_point.size() == (_vec.size()    + 1) );
        }

        // -----------------------------------------------------------------------------

        template<typename Point_t, typename Real_t>
        void Spline<Point_t, Real_t>::set_nodal_vector()
        {
            if( _node_type == ESpline::OPEN_UNIFORM)
                set_node_to_open_uniform();
            else if( _node_type == ESpline::UNIFORM )
                set_node_to_uniform();
        }

        // -----------------------------------------------------------------------------

        template<typename Point_t, typename Real_t>
        void Spline<Point_t, Real_t>::set_node_to_uniform()
        {
            const int n = _point.size() - 1;
            _node.resize( _k + n + 1 );

            Real_t step = (Real_t)1 / (Real_t)(n-_k+2);
            for (int i = 0; i < (int)_node.size(); ++i){
                _node[i] = ((Real_t)i) * step  - step * (Real_t)(_k-1);
            }
        }

        // -----------------------------------------------------------------------------

        template<typename Point_t, typename Real_t>
        void Spline<Point_t, Real_t>::set_node_to_open_uniform()
        {
            _node.resize( _k + _point.size() );

            int acc = 1;
            for (int i = 0; i < (int)_node.size(); ++i)
            {
                if(i < _k)
                    _node[i] = 0.;
                else if( i >= ((int)_point.size() + 1) )
                    _node[i] = 1.;
                else{
                    _node[i] = (Real_t)acc / (Real_t)(_point.size() + 1 - _k);
                    acc++;
                }
            }
        }

        // -----------------------------------------------------------------------------

        template<typename Point_t, typename Real_t>
        Point_t  Spline<Point_t, Real_t>::

        eval(Real_t u,
             const std::vector<Point_t>& point,
             int k,
             const std::vector<Real_t>& node,
             int off) const
        {
            assert( k > 1);
            assert((int)point.size() >= k );
            assert_splines();
            int dec = 0;
            // TODO: better search with dychotomi ?
            // TODO: check for overflow
            while( u > node[dec + k + off] )
                dec++;

            // TODO: use buffers in attributes for better performances ?
            std::vector<Point_t> p_rec(k, Point_t());
            for(int i = dec, j = 0; i < (dec + k); ++i, ++j)
                p_rec[j] = point[i];

            std::vector<Real_t> node_rec(k + k - 2, (Real_t)0);
            for(int i = (dec + 1), j = 0; i < (dec + k + k - 1); ++i, ++j)
                node_rec[j] = node[i + off];

            return eval_rec(u, p_rec, k, node_rec);
        }

        // -----------------------------------------------------------------------------

        template<typename Point_t, typename Real_t>
        Point_t Spline<Point_t, Real_t>::

        eval_rec(Real_t u,
                 std::vector<Point_t> p_in,
                 int k,
                 std::vector<Real_t> node_in) const
        {
            if(p_in.size() == 1)
                return p_in[0];

            // TODO: use buffers in attributes for better performances ?
            std::vector<Point_t> p_out(k - 1,  Point_t());
            for(int i = 0; i < (k - 1); ++i)
            {
                const Real_t n0 = node_in[i + k - 1];
                const Real_t n1 = node_in[i];
                const Real_t f0 = (n0 - u) / (n0 - n1);
                const Real_t f1 = (u - n1) / (n0 - n1);

                p_out[i] = p_in[i] *  f0 + p_in[i + 1] * f1;
            }

            std::vector<Real_t> node_out(node_in.size() - 2);
            for(int i = 1, j = 0; i < ((int)node_in.size()-1); ++i, ++j)
                node_out[j] = node_in[i];

            return eval_rec(u, p_out, (k - 1), node_out);
        }
    }
}
