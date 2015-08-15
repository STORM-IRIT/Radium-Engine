#include <Core/Utils/Any.hpp>

namespace Ra
{
    namespace Core
    {
        template <typename T>
        inline Any::Any(const T& t)
            : m_value(new AnyValue<T>(t))
        {
        }

        template <typename T>
        inline const T& Any::as() const
        {
            if (magicNumberFor<T>() != m_value->magicNumber)
            {
                // FIXME(Charly): Better way to handle those errors ?
                CORE_ERROR("Wrong cast types.");
            }

            return std::static_pointer_cast<const AnyValue<T>>(m_value)->value;
        }

        inline int Any::nextMagicNumer()
        {
            static int magic(0);
            return magic++;
        }

        template <typename T>
        inline int Any::magicNumberFor()
        {
            static int result(nextMagicNumber());
            return result;
        }

        template <typename T>
        inline Any::AnyValueBase::AnyValueBase(const int magic)
            : magicNumber(magic)
        {
        }

        template <typename T>
        inline Any::AnyValue<T>::AnyValue(const T& v)
            : Any::AnyValueBase(magicNumberFor<T>())
            , value(v)
        {
        }
    }
}