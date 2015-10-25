#include <Core/Containers/Grid3.hpp>

namespace Ra
{
    namespace Core
    {
        template<class T>
        Grid3<T>::
        
        Grid3(int x, int y, int z, const T* vals, const Ra::Core::Vector3i& pad) :
            _size(Ra::Core::Vector3i(x, y, z)),
            _pad_off(pad)
        {
            init_vals(vals);
        }
        
        // -----------------------------------------------------------------------------
        
        template<class T>
        Grid3<T>::
        
        Grid3(const Ra::Core::Vector3i& s, const T* vals, const Ra::Core::Vector3i& pad) :
            _size(s),
            _pad_off(pad)
        {
            init_vals(vals);
        }
        
        // -----------------------------------------------------------------------------
        
        template<class T>
        Grid3<T>::
        
        Grid3(const Ra::Core::Vector3i& size_, const T& val, const Ra::Core::Vector3i& pad) :
            _size( size_ ),
            _pad_off( pad ),
            _vals( size_.prod(), val)
        {
        
        }
        
        // -----------------------------------------------------------------------------
        
        template<class T>
        Grid3<T>::
        
        Grid3( const Grid3<T>& g ) :
            _size(g._size),
            _pad_off(g._pad_off)
        {
            init_vals(g._vals.data());
        }
        
        // -----------------------------------------------------------------------------
        
        template<class T>
        Grid3<T>::
        
        Grid3(const std::vector< Grid3<T>* >& list,
              const Ra::Core::Vector3i& max_s,
              std::vector<Idx3> &out_idx)
        {
            assert( list.size() );
            Ra::Core::Vector3i s = list[0]->_size;
        
            #ifndef NDEBUG
            assert( s(0) <= max_s(0) && s(1) <= max_s(1) && s(2) <= max_s(2) );
            // Every grid in list should have same size
            for (unsigned i = 1; i < list.size(); ++i) assert( list[i]->_size == s);
            #endif
        
            // compute the new grid dimensions
            unsigned int nb_max_x = max_s(0) / s(0);
            unsigned int nb_max_y = max_s(1) / s(1);
            unsigned int dim_x, dim_y, dim_z;
            if (nb_max_x > list.size()){
                dim_x = list.size();
                dim_y = 1;
                dim_z = 1;
            } else if (nb_max_x * nb_max_y > list.size()){
                dim_x = nb_max_x;
                dim_y = list.size() / nb_max_x + 1;
                dim_z = 1;
            } else {
                dim_x = nb_max_x;
                dim_y = nb_max_y;
                dim_z = list.size() / (nb_max_x*nb_max_y) + 1;
            }
        
            // concatenate input grids and compute out_idxs
            out_idx.clear();
            _size = Ra::Core::Vector3i(dim_x * s(0), dim_y * s(1), dim_z * s(2));
            _vals.resize( _size.prod() );
            Idx3 id(_size, 0);
            for(unsigned i = 0; i < list.size(); ++i)
            {
                Ra::Core::Vector3i v = id.to_3d();
                out_idx.push_back( Idx3(_size, v + list[i]->_pad_off) );
        
                for(Idx3 idx(s, 0); idx.is_in(); ++idx){
                    _vals[ (id+idx.to_3d()).to_linear() ] = list[i]->_vals[idx.to_linear()];
                }
                // put id to end of list[i] grid in _vals dims
                if (v(0) < _size(0) - s(0))
                    id = id + Idx3(s, s(0)-1, 0, 0).to_3d();
                else if (v(0) < _size(0) - s(0))
                    id = id + Idx3(s, s(0)-1, s(1)-1, 0).to_3d();
                else
                    id = id + Idx3(s, s(0)-1, s(1)-1, s(2)-1).to_3d();
                // increment id to ensure it points to the begin of list[i+1]
                ++id;
            }
            _pad_off = Ra::Core::Vector3i::Zero();
        }
        
        // -----------------------------------------------------------------------------
        
        template <class T>
        void Grid3<T>::padd(int padding, Pad_t type, T val)
        {
            Ra::Core::Vector3i s = _size + Ra::Core::Vector3i(padding * 2, padding * 2, padding * 2);
        
            assert( _size(0) < s(0) && _size(1) < s(1) && _size(2) < s(2) );
        
            // get offsets for padding;
            Ra::Core::Vector3i off = (s - _size) / 2;
            Idx3 offset(s, off);
            // do custom padding / init p_v
            std::vector<T> p_v(s(0) * s(1) * s(2), val);
            // copy vals
            for (Idx3 id(_size, 0); id.is_in(); id++ ){
                int i = (id.to_3d() + offset).to_linear();
                p_v[ i ] = _vals[ id.to_linear() ];
            }
            // do copy padding
            if (type == COPY)
            {
                int dx = s(0) - off(0);
                int dy = s(1) - off(1);
                int dz = s(2) - off(2);
                // incremental faces extrusion : z-axis, y-axis, x-axis
                for (Idx3 id(Ra::Core::Vector3i(_size(0), _size(1), off(2)), 0); id.is_in(); ++id){
                    Ra::Core::Vector3i vid = id.to_3d();
                    p_v[ (vid + Idx3(s, off(0), off(1),  0)).to_linear() ] = p_v[ Idx3(s, off(0) + vid(0), off(1) + vid(1), off(2)).to_linear() ];
                    p_v[ (vid + Idx3(s, off(0), off(1), dz)).to_linear() ] = p_v[ Idx3(s, off(0) + vid(0), off(1) + vid(1),  dz-1).to_linear() ];
                }
                for (Idx3 id(Ra::Core::Vector3i(_size(0), off(1), s(2)), 0); id.is_in(); ++id){
                    Ra::Core::Vector3i vid = id.to_3d();
                    p_v[ (vid + Idx3(s, off(0),  0, 0)).to_linear() ] = p_v[ Idx3(s, off(0) + vid(0), off(1) , vid(2)).to_linear() ];
                    p_v[ (vid + Idx3(s, off(0), dy, 0)).to_linear() ] = p_v[ Idx3(s, off(0) + vid(0), dy - 1, vid(2)).to_linear() ];
                }
                for (Idx3 id(Ra::Core::Vector3i(off(0), s(1), s(2)), 0); id.is_in(); ++id){
                    Ra::Core::Vector3i vid = id.to_3d();
                    p_v[ (vid + Idx3(s,  0, 0, 0)).to_linear() ] = p_v[ Idx3(s, off(0), vid(1), vid(2)).to_linear() ];
                    p_v[ (vid + Idx3(s, dx, 0, 0)).to_linear() ] = p_v[ Idx3(s,  dx-1, vid(1), vid(2)).to_linear() ];
                }
            }
            // update 'this'
            _vals.clear();
            _vals.swap( p_v );
            _size = s;
            _pad_off += off;
        }
        
        // -----------------------------------------------------------------------------
        
        template <class T>
        void Grid3<T>::
        
        init_vals(const T* vals)
        {
            int nb_elt = _size.prod();
            _vals.resize( nb_elt );
            for (int i = 0; i < nb_elt; ++i) {
                _vals[i] = vals[i];
            }
        }
        
        // -----------------------------------------------------------------------------
        
        template <class T>
        Grid3<T>& Grid3<T>::operator= (const Grid3<T>& cp){
            _size    = cp._size;
            _pad_off = cp._pad_off;
            _vals    = cp._vals;
            return *this;
        }
        
        
        // -----------------------------------------------------------------------------
        
        template <class T>
        const T& Grid3<T>::operator() (const Ra::Core::Vector3i& coord) const {
            return (*this)(coord(0), coord(1), coord(2));
        }
        
        // -----------------------------------------------------------------------------
        
        template <class T>
        const T& Grid3<T>::operator() (int x, int y, int z) const {
            Idx3 idx = Idx3(_size, _pad_off) + Ra::Core::Vector3i(x, y, z);
            assert( idx.to_linear() < _size.product() );
            return _vals[idx.to_linear()];
        }
        
        // -----------------------------------------------------------------------------
        
        template <class T>
        const T& Grid3<T>::operator() (const Idx3& idx) const {
            // Otherwise it doesn't make sense
            assert( idx.size() == _size );
            Idx3 i = Idx3(_size, _pad_off) + idx.to_3d();
            assert( i.to_linear() < _size.product() );
            return _vals[i.to_linear()];
        }
        
        // -----------------------------------------------------------------------------
        
        template <class T>
        T& Grid3<T>::operator() (const Ra::Core::Vector3i& coord) {
            return (*this)(coord(0), coord(1), coord(2));
        }
        
        // -----------------------------------------------------------------------------
        
        template <class T>
        T& Grid3<T>::operator() (int x, int y, int z) {
            Idx3 idx = Idx3(_size, _pad_off) + Ra::Core::Vector3i(x, y, z);
            assert( idx.to_linear() < _size.product() );
            return _vals[idx.to_linear()];
        }
        
        // -----------------------------------------------------------------------------
        
        template <class T>
        T& Grid3<T>::operator() (const Idx3& idx) {
            // Otherwise it doesn't make sense
            assert( idx.size() == _size );
            Idx3 i = Idx3(_size, _pad_off) + idx.to_3d();
            assert( i.to_linear() < _size.product() );
            return _vals[i.to_linear()];
        }    
    }
}
