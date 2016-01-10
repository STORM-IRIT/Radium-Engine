#ifndef RADIUMENGINE_SINGLETON_HPP
#define RADIUMENGINE_SINGLETON_HPP

#include <Core/RaCore.hpp>

// Singleton utility.
// I used to be a template like you, then I took a DLL to the knee...

/// Add this macro (followed by a semicolon) in your class header.
/// The macro should appear first in the class, before any public: 
/// specifier.
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



/// Add this macro in the singleton cpp, followed by a semicolon.
// Limitations : TYPE cannot be a nested type 
// RA_SINGLETON_IMPLEMENTATION(A::MySingleton); will *not* work.
#define RA_SINGLETON_IMPLEMENTATION(TYPE)                        \
namespace TYPE##NS { TYPE* s_instance = nullptr;}                \
void TYPE::replaceInstance(TYPE* p) { delete TYPE##NS::s_instance; TYPE##NS::s_instance= p; } \
TYPE* TYPE::getInstance() { return TYPE##NS::s_instance; }       \
void TYPE::destroyInstance() { delete TYPE##NS::s_instance; TYPE##NS::s_instance = nullptr; }\
class TYPE
// The line above is just there to make the macro end with a ;


#endif // RADIUMENGINE_SINGLETON_HPP
