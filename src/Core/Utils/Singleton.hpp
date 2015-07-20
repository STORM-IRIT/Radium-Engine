#ifndef RADIUMENGINE_SINGLETON_HPP
#define RADIUMENGINE_SINGLETON_HPP

#include <cstdio>
#include <Core/CoreMacros.hpp>

namespace Ra { namespace Core
{

    template<typename T>
    class Singleton
    {
    public:
        template<typename... Args>
        static T* createInstance(const Args& ... args)
        {
            CORE_ASSERT(nullptr == s_instance, "Singleton has already been created");
            s_instance = new T(args...);
            return s_instance;
        }

        static T* getInstancePtr()
        {
            CORE_ASSERT(s_instance, "Singleton is uninitialized.");
            return s_instance;
        }

        static T& getInstanceRef()
        {
            CORE_ASSERT(s_instance, "Singleton is uninitialized.");
            return *s_instance;
        }

        static void destroyInstance()
        {
            if (s_instance)
            {
                delete s_instance;
                s_instance = nullptr;
            }
        }

    protected:
        Singleton() { }

        ~Singleton() { }

    private:
        Singleton(const Singleton<T>&) = delete;

        void operator=(const Singleton<T>&) = delete;

    private:
        static T* s_instance;
    };

    template<typename T> T* Singleton<T>::s_instance = nullptr;

}} // namespace Ra::Core

#endif // RADIUMENGINE_SINGLETON_HPP
