#ifndef RADIUMENGINE_CORE_ALIGNED_STD_VECTOR_HPP_
#define RADIUMENGINE_CORE_ALIGNED_STD_VECTOR_HPP_
#include <Core/RaCore.hpp>

#include <vector>
#include <Core/Containers/AlignedAllocator.hpp>

namespace Ra
{
    namespace Core
    {

        /// Shortcut for the ubiquitous 16-byte aligned std::vector
        template <typename T, uint Align = 16>
        class AlignedStdVector : public std::vector <T, AlignedAllocator <T, Align>>
        {
            using std::vector<T, AlignedAllocator <T, Align>>::vector;
        };
    }
}
#endif //RADIUMENGINE_CORE_ALIGNED_STD_VECTOR_HPP_
