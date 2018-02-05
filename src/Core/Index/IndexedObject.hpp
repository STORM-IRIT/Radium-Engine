#ifndef RADIUMENGINE_INDEXEDOBJECT_HPP
#define RADIUMENGINE_INDEXEDOBJECT_HPP

#include <Core/RaCore.hpp>
#include <Core/Index/Index.hpp>

namespace Ra
{
    namespace Core
    {

        class DLL_EXPORT IndexedObject
        {
        public:
            /// CONSTRUCTOR
            explicit inline  IndexedObject( Index idx = Index::Invalid() )
            {
                this->idx = idx;
            }
            inline IndexedObject( const IndexedObject& id_obj )
            {
                idx = id_obj.idx;
            }

            /// DESTRUCTOR
            virtual inline ~IndexedObject() { }

            /// VARIABLE
            Index idx;
        };

    }
} // namespace Ra::Core

#endif // RADIUMENGINE_INDEXEDOBJECT_HPP
