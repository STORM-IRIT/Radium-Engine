#ifndef RADIUMENGINE_ANY_HPP
#define RADIUMENGINE_ANY_HPP

#include <Core/CoreMacros.hpp>

#include <memory>

namespace Ra
{
    namespace Core
    {
        class Any
        {
        public:
            template <typename T>
            inline RA_API Any(const T& t);

            template <typename T>
            inline RA_API const T& as() const;

        private:
            static inline int nextMagicNumer();

            template <typename T>
            static inline int magicNumberFor();

            struct AnyValueBase
            {
                int magicNumber;
                
                inline explicit AnyValueBase(const int magic);
                inline virtual ~AnyValueBase() {}
            };

            template <typename T>
            struct AnyValue : AnyValueBase
            {
                T value;

                inline AnyValue(const T& v);
            };

        private:
            std::shared_ptr<AnyValueBase> m_value;
        };
    }
}

#include <Core/Utils/Any.inl>

#endif // RADIUMENGINE_ANY_HPP