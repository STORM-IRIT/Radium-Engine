#ifndef RADIUMENGINE_INTERFACE_HPP
#define RADIUMENGINE_INTERFACE_HPP

#include <cstring>

#include <QtPlugin>

namespace Ra
{
    namespace Engine
    {
        namespace Plugin
        {

            class Interface
            {
            public:
                /// ENUM
                enum class StatusType
                {
                    IDLE,
                    LOADING,
                    READY,
                    EXECUTION
                };

                /// CONSTRUCTOR
                Interface() : m_status( StatusType::IDLE ) { }

                /// DESTRUCTOR
                virtual ~Interface() { }

                /// INTERFACE
                virtual std::string getName() const = 0;
                virtual void            run() = 0;

                /// STATUS
                inline StatusType getStatus() const
                {
                    return m_status;
                }

            protected:
                /// STATUS
                inline void setStatus( const StatusType type )
                {
                    m_status = type;
                }

            private:
                /// VARIABLE
                StatusType m_status;

            };

        } // namespace Plugin
    } // namespace Engine
} // namespace Ra

#define INTERFACE_IID "Radium.Engine.Plugins.Interface"
Q_DECLARE_INTERFACE( Ra::Engine::Plugin::Interface, INTERFACE_IID )

#endif // RADIUMENGINE_INTERFACE_HPP
