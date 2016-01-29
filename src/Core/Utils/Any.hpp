// <experimental/Any> -*- C++ -*-

// Copyright (C) 2014-2015 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// Any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT Any WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file experimental/any
 *  This is a TS C++ Library header.
 */

#ifndef RADIUMENGINE_ANY_HPP
#define RADIUMENGINE_ANY_HPP

#include <typeinfo>
#include <new>
#include <utility>
#include <type_traits>

#include <Core/CoreMacros.hpp>

#ifndef OS_WINDOWS
using std::type_info; // Use type_info instead of std::type_info in order to build properly with MSVC
#endif

namespace Ra {
    namespace Core {
        template<typename...> struct Or;
        template<> struct Or<> : public std::false_type { };
        template<typename T> struct Or<T> : public T { };
        template<typename T, typename U> struct Or<T, U> : public std::conditional<T::value, U, U>::type { };

        template<typename T, typename U, typename V, typename... Tn>
        struct Or<T, U, V, Tn...> : public std::conditional<T::value, T, Or<U, V, Tn...>>::type { };

        /**
         *  @brief A type-safe container of any type.
         *
         *  An @c any object's state is either empty or it stores a contained object
         *  of CopyConstructible type.
         */
        class Any
        {
            // Holds either pointer to a heap object or the contained object itself.
            union Storage
            {
                void* m_ptr;
                std::aligned_storage<sizeof( void* ), sizeof( void* )>::type m_buffer;
            };

            template < typename T, typename Safe = std::is_trivially_copyable<T>,
                       bool TFits = ( sizeof( T ) <= sizeof( Storage ) ) >
                       using Internal = std::integral_constant < bool, Safe::value && TFits >;

            template<typename T>
            struct ManagerInternal; // uses small-object optimization

            template<typename T>
            struct ManagerExternal; // creates contained object on the heap

            template<typename T>
            using Manager = std::conditional_t<Internal<T>::value,
                                               ManagerInternal<T>,
                                               ManagerExternal<T>>;

            template<typename T, typename Decayed = std::decay_t<T>>
            using Decay = std::enable_if_t < !std::is_same<Decayed, Any>::value, Decayed >;

        public:
            // construct/destruct

            /// Default constructor, creates an empty object.
            Any() noexcept 
                : m_manager( nullptr ) { }

            /// Copy constructor, copies the state of @p other
            Any( const Any& other ) 
                : m_manager( other.m_manager )
            {
                if ( !other.empty() )
                {
                    Arg arg;
                    arg.m_any = this;
                    m_manager( OP_CLONE, &other, &arg );
                }
            }

            /**
             * @brief Move constructor, transfer the state from @p other
             *
             * @post @c other.empty() (not guaranteed for other implementations)
             */
            Any( Any&& other ) noexcept 
                : m_manager( other.m_manager )
                , m_storage( other.m_storage )
            {
                other.m_manager = nullptr;
            }

            /// Construct with a copy of @p value as the contained object.
            template <typename V, typename T = Decay<V>, typename _Mgr = Manager<T>> Any( V && value )
                : m_manager( &_Mgr::manager )
                , m_storage( _Mgr::create( std::forward<V>( value ) ) )
            {
                static_assert( std::is_copy_constructible<T>::value,
                               "The contained object must be CopyConstructible" );
            }

            /// Destructor, calls @c clear()
            ~Any()
            {
                clear();
            }

            // assignments

            /// Copy the state of
            Any& operator=( const Any& rhs )
            {
                Any( rhs ).swap( *this );
                return *this;
            }

            /**
             * @brief Move assignment operator
             *
             * @post @c rhs.empty() (not guaranteed for other implementations)
             */
            Any& operator=( Any && rhs ) noexcept
            {
                Any( std::move( rhs ) ).swap( *this );
                return *this;
            }

            /// Store a copy of @p rhs as the contained object.
            template<typename V> Any& operator=( V && rhs )
            {
                Any( std::forward<V>( rhs ) ).swap( *this );
                return *this;
            }

            // modifiers

            /// If not empty, destroy the contained object.
            void clear() noexcept
            {
                if ( !empty() )
                {
                    m_manager( OP_DESTROY, this, nullptr );
                    m_manager = nullptr;
                }
            }

            /// Exchange state with another object.
            void swap( Any& rhs ) noexcept
            {
                std::swap( m_manager, rhs.m_manager );
                std::swap( m_storage, rhs.m_storage );
            }

            // observers

            /// Reports whether there is a contained object or not.
            bool empty() const noexcept
            {
                return m_manager == nullptr;
            }

            template<typename T> 
            static constexpr bool isValidCast()
            {
                return Or<std::is_reference<T>, std::is_copy_constructible<T>>::value;
            }

        private:
            enum Op { OP_ACCESS, OP_GETTYPEINFO, OP_CLONE, OP_DESTROY };

            union Arg
            {
                void* m_obj;
                const type_info* m_typeinfo;
                Any* m_any;
            };

            void ( *m_manager )( Op, const Any*, Arg* );
            Storage m_storage;

            template<typename T> friend const T* anyCast( const Any* any ) noexcept;
            template<typename T> friend T* anyCast( Any* any ) noexcept;
            
            template<typename T> 
            static void* anyCaster( const Any* any )
            {
                if ( any->m_manager != &Manager<std::decay_t<T>>::manager )
                {
                    return nullptr;
                }
                Arg arg;
                any->m_manager( OP_ACCESS, any, &arg );
                return arg.m_obj;
            }

            // Manage in-place contained object.
            template<typename T> 
            struct ManagerInternal
            {
                static void manager( Op which, const Any* anyp, Arg* arg );

                template<typename _Up> 
                static Storage create( _Up&& value )
                {
                    Storage storage;
                    void* addr = &storage.m_buffer;
                    ::new( addr ) T( std::forward<_Up>( value ) );
                    return storage;
                }

                template<typename _Alloc, typename _Up> 
                static Storage alloc( const _Alloc&, _Up&& value )
                {
                    return create( std::forward<_Up>( value ) );
                }
            };

            // Manage external contained object.
            template<typename T>
            struct ManagerExternal
            {
                static void manager( Op which, const Any* anyp, Arg* arg );

                template<typename _Up> 
                static Storage create( _Up&& value )
                {
                    Storage storage;
                    storage.m_ptr = new T( std::forward<_Up>( value ) );
                    return storage;
                }
            };
        };

        /// Exchange the states of two @c any objects.
        inline void swap( Any& x, Any& y ) noexcept { x.swap( y ); }

        /**
         * @brief Access the contained object.
         *
         * @tparam  V  A const-reference or CopyConstructible type.
         * @param   any       The object to access.
         * @return  The contained object.
         * @throw   bad_anyCast If <code>
         *          any.type() != typeid(remove_reference_t<V>)
         *          </code>
         */
        template<typename V> 
        inline V anyCast( const Any& any )
        {
            static_assert( Any::isValidCast<V>(),
                           "Template argument must be a reference or CopyConstructible type" );
            auto p = anyCast<std::add_const_t<std::remove_reference_t<V>>>( &any );
            if ( p )
            {
                return *p;
            }
            CORE_ERROR( "Could not cast given any to required type." );

            // prevent clang warning -Wreturn-type
            return V();
        }

        /**
         * @brief Access the contained object.
         *
         * @tparam  V  A reference or CopyConstructible type.
         * @param   any       The object to access.
         * @return  The contained object.
         * @throw   bad_anyCast If <code>
         *          any.type() != typeid(remove_reference_t<V>)
         *          </code>
         *
         * @{
         */
        template<typename V>
        inline V anyCast( Any& any )
        {
            static_assert( Any::isValidCast<V>(),
                           "Template argument must be a reference or CopyConstructible type" );
            auto p = anyCast<std::remove_reference_t<V>>( &any );
            if ( p )
            {
                return *p;
            }
            CORE_ERROR( "Could not cast given any to required type." );

            // prevent clang warning -Wreturn-type
            return V();
        }

        template<typename V> 
        inline V anyCast( Any&& any )
        {
            static_assert( Any::isValidCast<V>(),
                           "Template argument must be a reference or CopyConstructible type" );
            auto p = anyCast<std::remove_reference_t<V>>( &any );
            if ( p )
            {
                return *p;
            }
            CORE_ERROR( "Could not cast given any to required type." );

            // prevent clang warning -Wreturn-type
            return V();
        }
        // @}

        /**
         * @brief Access the contained object.
         *
         * @tparam  V  The type of the contained object.
         * @param   any       A pointer to the object to access.
         * @return  The address of the contained object if <code>
         *          any != nullptr && any.type() == typeid(V)
         *          </code>, otherwise a null pointer.
         *
         * @{
         */
        template<typename V> 
        inline const V* anyCast( const Any* any ) noexcept
        {
            if ( any )
            {
                return static_cast<V*>( Any::anyCaster<V>( any ) );
            }
            return nullptr;
        }

        template<typename V> 
        inline V* anyCast( Any* any ) noexcept
        {
            if ( any )
            {
                return static_cast<V*>( Any::anyCaster<V>( any ) );
            }
            return nullptr;
        }
        // @}

        template<typename T> 
        void Any::ManagerInternal<T>::manager( Op which, const Any* any, Arg* arg )
        {
            // The contained object is in m_storage.m_buffer
            auto ptr = reinterpret_cast<const T*>( &any->m_storage.m_buffer );
            switch ( which )
            {
                case OP_ACCESS:
                    arg->m_obj = const_cast<T*>( ptr );
                    break;
                case OP_GETTYPEINFO:
                    break;
                case OP_CLONE:
                    ::new( &arg->m_any->m_storage.m_buffer ) T( *ptr );
                    break;
                case OP_DESTROY:
                    ptr->~T();
                    break;
            }
        }

        template<typename T> 
        void Any::ManagerExternal<T>::manager( Op which, const Any* any, Arg* arg )
        {
            // The contained object is *m_storage.m_ptr
            auto ptr = static_cast<const T*>( any->m_storage.m_ptr );
            switch ( which )
            {
                case OP_ACCESS:
                    arg->m_obj = const_cast<T*>( ptr );
                    break;
                case OP_GETTYPEINFO:
                    break;
                case OP_CLONE:
                    arg->m_any->m_storage.m_ptr = new T( *ptr );
                    break;
                case OP_DESTROY:
                    delete ptr;
                    break;
            }
        }
        
        template<typename T> 
        bool isOfType( const Any& any, const T& dummy = T() )
        {
            return anyCast<std::add_const_t<std::remove_reference_t<T>>>( &any );
        }

        template<typename T> 
        bool isOfType( Any& any, const T& dummy = T() )
        {
            return anyCast<std::remove_reference_t<T>>( &any );
        }

    } // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_ANY_HPP




