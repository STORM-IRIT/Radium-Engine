#ifndef RADIUMENGINE_MAKESHARED_HPP_
#define RADIUMENGINE_MAKESHARED_HPP_

#include <memory>

namespace Ra {
    namespace Core {

        /// A replacement for std::make_shared on platforms where it
        /// does not respect aligments.
        template < typename T, class... Args >
        inline std::shared_ptr<T> make_shared( Args&&... args )
        {
#if 0       // use std::make_shared

            return std::make_shared<T>(args...);

#else       // use new and shared_ptr constructor.

            return std::shared_ptr<T>( new T (args...) );

#endif
        }
    }
}


#endif // RADIUMENGINE_MAKESHARED_HPP_
