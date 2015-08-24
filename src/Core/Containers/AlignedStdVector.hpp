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
        template <typename T>
        class AlignedStdVector : public std::vector <T, AlignedAllocator <T, 16>>
        {
            using std::vector<T, AlignedAllocator <T, 16>>::vector;
        };
    }
}
#endif //RADIUMENGINE_CORE_ALIGNED_STD_VECTOR_HPP_
