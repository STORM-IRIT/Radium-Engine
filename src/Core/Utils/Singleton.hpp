#pragma once

#include <Core/RaCore.hpp>

#include <memory>

// Singleton utility.
// This file give you two macros to automatically implement a class
// as a singleton.
// I used to be a template like you, then I took a DLL to the knee...

// Usage : The singleton instance is initially set to null.
// To create the instance, call `createInstance( )` with the
// class constructor arguments.
// To access the singleton instance, call getInstance().
// The singleton instance can also be destroyed (reset to null).

/// Add this macro (followed by a semicolon) in your class header.
/// The macro should appear first in the class, before any public:
/// specifier.
#define RA_SINGLETON_INTERFACE( TYPE )                                     \
  public:                                                                  \
    template <typename... Args>                                            \
    static TYPE* createInstance( const Args&... args ) {                   \
        replaceInstance( std::move( std::make_unique<TYPE>( args... ) ) ); \
        return getInstance();                                              \
    }                                                                      \
    static TYPE* getInstance();                                            \
    static void destroyInstance();                                         \
                                                                           \
  protected:                                                               \
    static void replaceInstance( std::unique_ptr<TYPE>&& ptr );            \
    TYPE( const TYPE& ) = delete;                                          \
    void operator=( const TYPE& ) = delete

/// Add this macro in the singleton cpp, followed by a semicolon.
// Limitations : TYPE cannot be a nested type
// RA_SINGLETON_IMPLEMENTATION(A::MySingleton); will *not* work.
#define RA_SINGLETON_IMPLEMENTATION( TYPE )                                 \
    namespace TYPE##NS {                                                    \
        std::unique_ptr<TYPE> s_instance { nullptr };                       \
    }                                                                       \
    void TYPE::replaceInstance( std::unique_ptr<TYPE>&& ptr ) {             \
        TYPE##NS::s_instance = std::move( ptr );                            \
    }                                                                       \
    TYPE* TYPE::getInstance() { return TYPE##NS::s_instance.get(); }        \
    void TYPE::destroyInstance() { TYPE##NS::s_instance.reset( nullptr ); } \
    class TYPE
// The line above is just there to make the macro end with a ;
