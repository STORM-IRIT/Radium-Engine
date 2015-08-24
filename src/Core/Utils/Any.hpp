#ifndef RADIUMENGINE_ANY_HPP
#define RADIUMENGINE_ANY_HPP

#include <Core/RaCore.hpp>

#include <memory>

namespace Ra
{
    namespace Core
    {
        template <typename T>
        inline int magicNumberFor();
        DLL_EXPORT extern int nextMagicNumber();

        class Any
        {
        public:
            template <typename T>
            inline Any( const T& t );

            template <typename T>
            inline const T& as() const;

            inline int getType() const
            {
                return m_value->magicNumber;
            }

            template <typename T>
            static inline int getDeclTypeFor()
            {
                return magicNumberFor<T>();
            }

        private:
            struct AnyValueBase
            {
                int magicNumber;

                inline explicit AnyValueBase( const int magic ) : magicNumber( magic ) {}
                inline virtual ~AnyValueBase() {}
            };

            template <typename T>
            struct AnyValue : AnyValueBase
            {
                T value;

                inline AnyValue( const T& v );
            };

        private:
            std::shared_ptr<AnyValueBase> m_value;
        };


    }
}

#include <Core/Utils/Any.inl>

#endif // RADIUMENGINE_ANY_HPP