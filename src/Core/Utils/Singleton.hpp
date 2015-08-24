#ifndef RADIUMENGINE_SINGLETON_HPP
#define RADIUMENGINE_SINGLETON_HPP

#include <Core/RaCore.hpp>

// Singleton utility.
// I used to be a template like you, then I took a DLL to the knee...

/// Add this macro (followed by a semicolon) in your class header.
#define RA_SINGLETON_INTERFACE(TYPE)                    \
public:                                                 \
    static void replaceInstance(TYPE*);                 \
    template<typename... Args>                          \
    static TYPE* createInstance(const Args& ... args)   \
    {                                                   \
        replaceInstance(new TYPE(args...));             \
        return getInstance();                           \
    }                                                   \
    static TYPE* getInstance();                         \
    static void destroyInstance();                      \
protected:                                              \
    TYPE(const TYPE&) = delete;                         \
    void operator=(const TYPE&) = delete



/// Add this macro in the singleton cpp, without a semicolon.
#define RA_SINGLETON_IMPLEMENTATION(TYPE)               \
namespace {static TYPE* s_instance = nullptr;}          \
void TYPE::replaceInstance(TYPE* p) { s_instance = p; } \
TYPE* TYPE::getInstance() { return s_instance; }        \
void TYPE::destroyInstance() { delete s_instance; s_instance = nullptr; }


#endif // RADIUMENGINE_SINGLETON_HPP
