#ifndef GRID_ENUMS_HPP
#define GRID_ENUMS_HPP

namespace Ra
{
    namespace Core
    {
        /// @brief define the span of the subgrid
        struct Range
        {
            Range() : _dyn_range(true), _a(0), _b(-1) { }
        
            /// @param start, end: staring index and end index (both included)
            Range(int start, int end) : _dyn_range(false), _a( start ), _b( end+1 ) { }
        
            int nb_elts() const { return _b-_a; }
        
            /// If false you should not consider _a and _b but the size of the object
            bool _dyn_range;
        
            int _a; ///< starting index (included)
            int _b; ///< last index (excluded)
        };
                
        /// @brief select the whole span of the grid
        struct All : public Range
        {
            All() : Range()
            {
            }
        };    
    }
}

#endif
