#include <Core/Containers/Grid2.hpp>

namespace Ra
{
    namespace Core
    {    
        static inline bool check_bounds(const Range& r, int size){
            return r._a >= 0 && r._a < size && (r._b - 1) >= 0 && (r._b - 1) < size;
        }
        
        // -----------------------------------------------------------------------------
        // Implemens of Grid3 to avoid cross definitions
        // -----------------------------------------------------------------------------
        
        template< typename T>
        Grid2_ref<T>
        
        Grid3<T>::operator() (Range x, Range y, int z) {
            assert( z < size().z );
            if( x._dyn_range ) x._b = size().x;
            if( y._dyn_range ) y._b = size().y;
            assert( check_bounds(x, size().x) );
            assert( check_bounds(y, size().y) );
            return Grid2_ref<T>::make_xy(*this, x, y, z);
        }
        
        //------------------------------------------------------------------------------
        
        template< typename T>
        Grid2_ref<T>
        
        Grid3<T>::operator() (Range x, int y  , Range z){
            assert( y < size().y );
            if( x._dyn_range ) x._b = size().x;
            if( z._dyn_range ) z._b = size().z;
            assert( check_bounds(x, size().x) );
            assert( check_bounds(z, size().z) );
            return Grid2_ref<T>::make_xz(*this, x, y, z);
        }
        
        //------------------------------------------------------------------------------
        
        template< typename T>
        Grid2_ref<T>
        
        Grid3<T>::operator() (int x  , Range y, Range z){
            assert( x < size().x );
            if( y._dyn_range ) y._b = size().y;
            if( z._dyn_range ) z._b = size().z;
            assert( check_bounds(y, size().y) );
            assert( check_bounds(z, size().z) );
            return Grid2_ref<T>::make_yz(*this, x, y, z);
        }
        
        //------------------------------------------------------------------------------
        
        template< typename T>
        const Grid2_const_ref<T>
        
        Grid3<T>::operator() (Range x, Range y, int z) const {
            assert( z < size().z );
            if( x._dyn_range ) x._b = size().x;
            if( y._dyn_range ) y._b = size().y;
            assert( check_bounds(x, size().x) );
            assert( check_bounds(y, size().y) );
            return Grid2_const_ref<T>::make_xy(*this, x, y, z);
        }
        
        //------------------------------------------------------------------------------
        
        template< typename T>
        const Grid2_const_ref<T>
        
        Grid3<T>::operator() (Range x, int y  , Range z) const {
            assert( y < size().y );
            if( x._dyn_range ) x._b = size().x;
            if( z._dyn_range ) z._b = size().z;
            assert( check_bounds(x, size().x) );
            assert( check_bounds(z, size().z) );
            return Grid2_const_ref<T>::make_xz(*this, x, y, z);
        }
        
        //------------------------------------------------------------------------------
        
        template< typename T>
        const Grid2_const_ref<T>
        
        Grid3<T>::operator() (int x, Range y, Range z) const {
            assert( x < size().x );
            if( y._dyn_range ) y._b = size().y;
            if( z._dyn_range ) z._b = size().z;
            assert( check_bounds(y, size().y) );
            assert( check_bounds(z, size().z) );
            return Grid2_const_ref<T>::make_yz(*this, x, y, z);
        }
        
        // -----------------------------------------------------------------------------
        // Implemens of Grid2 to avoid cross definitions
        // -----------------------------------------------------------------------------
        
        template< typename T>
        Grid2<T>::Grid2(const Ra::Core::Vector2i& size_,
                              const T& val,
                              const Ra::Core::Vector2i& pad) :
            Grid2_ref<T>( *this ),
            _vals( size_.prod(), val),
            _size( size_ ),
            _pad_off( pad )
        {
        
        }
        
        // -----------------------------------------------------------------------------
        
        template< typename T>
        Grid2<T>::Grid2(const Ra::Core::Vector2i& s,
                        const T* vals,
                        const Ra::Core::Vector2i& pad) :
            Grid2_ref<T>( *this ),
            _size( s ),
            _pad_off( pad )
        {
            init_vals( vals );
        }
        
        // -----------------------------------------------------------------------------
        
        template< typename T>
        Grid2<T>::Grid2(const Grid2& cp) :
            Grid2_ref<T>( *this ),
            _size( cp._size ),
            _pad_off( cp._pad_off ),
            _vals( _size.prod() )
        {
            for( Idx2 idx(_size, 0); idx.is_in(); ++idx )
                _vals[idx.to_linear()] = cp( idx );
        }
        
        // -----------------------------------------------------------------------------
        
        template< typename T>
        Grid2<T>& Grid2<T>::operator=(Grid2_const_ref<T> cp)
        {
            _size    = cp.alloc_size();
            _pad_off = cp.get_padd_offset();
            _vals.resize( _size.prod() );
            for( Idx2 idx(_size, 0); idx.is_in(); ++idx )
                _vals[idx.to_linear()] = cp( idx );
        
            return *this;
        }
        
        // -----------------------------------------------------------------------------
        
        template< typename T>
        Grid2<T>& Grid2<T>::operator= (const Grid2<T>& cp)
        {
            _size    = cp._size;
            _pad_off = cp._pad_off;
            _vals.resize( _size.prod() );
            for( Idx2 idx(_size, 0); idx.is_in(); ++idx )
                _vals[idx.to_linear()] = cp( idx );
            return *this;
        }
        
        // -----------------------------------------------------------------------------
        
        // Implem note:
        // non const 2D accessor accessors must use this one so that everyone
        // behave the same when get_val is overriden
        template< typename T>
        T& Grid2<T>::get_val(int x, int y)
        {
            Idx2 idx = Idx2(_size, _pad_off) + Ra::Core::Vector2i(x, y);
            return _vals[idx.to_linear()];
        }
        
        // -----------------------------------------------------------------------------
        
        // Implem note:
        // const 2D accessor accessors must use this one so that everyone
        // behave the same when get_val is overriden
        template< typename T>
        const T& Grid2<T>::get_val(int x, int y) const
        {
            Idx2 idx = Idx2(_size, _pad_off) + Ra::Core::Vector2i(x, y);
            return _vals[idx.to_linear()];
        }
        
        // -----------------------------------------------------------------------------
        
        template< typename T>
        T& Grid2<T>::operator() (int x, int y) { return get_val(x, y); }
        
        // -----------------------------------------------------------------------------
        
        template< typename T>
        T& Grid2<T>::operator() ( const Idx2& idx )
        {
            Ra::Core::Vector2i v = idx.to_2d();
            // We use the virtual operator as it might be overidden.
            return get_val(v(0), v(1));
        }
        
        // -----------------------------------------------------------------------------
        
        template< typename T>
        T& Grid2<T>::operator() ( const Ra::Core::Vector2i& pos ) {
            // We use the virtual operator as it might be overidden.
            return get_val(pos(0), pos(1));
        }
        
        // -----------------------------------------------------------------------------
        
        template< typename T>
        Grid2<T>& Grid2<T>::operator() (Range, Range) { return (*this); }
        
        // -----------------------------------------------------------------------------
        
        template< typename T>
        const T& Grid2<T>::operator() (int x, int y) const { return get_val(x, y); }
        
        // -----------------------------------------------------------------------------
        
        template< typename T>
        const T& Grid2<T>::operator() ( const Idx2& idx ) const {
            Ra::Core::Vector2i v = idx.to_2d();
            // We use the virtual operator as it might be overidden.
            return get_val(v(0), v(1));
        }
        
        // -----------------------------------------------------------------------------
        
        template< typename T>
        const T& Grid2<T>::operator() ( const Ra::Core::Vector2i& pos ) const {
            // We use the virtual operator as it might be overidden.
            return get_val(pos(0), pos(1));
        }
        
        // -----------------------------------------------------------------------------
        
        template< typename T>
        const Grid2<T>& Grid2<T>::operator() (Range, Range) const {
            return (*this);
        }
        
        // -----------------------------------------------------------------------------
        
        template< typename T>
        void Grid2<T>::init_vals(const T* vals)
        {
            int nb_elt = _size.prod();
            _vals.resize( nb_elt );
            for (int i = 0; i < nb_elt; ++i)
                _vals[i] = vals[i];
        }
    }
}
