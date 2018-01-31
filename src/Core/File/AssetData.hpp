#ifndef RADIUMENGINE_ASSET_DATA_HPP
#define RADIUMENGINE_ASSET_DATA_HPP

#include <Core/RaCore.hpp>
#include <string>

namespace Ra {
    namespace Asset {

        class RA_CORE_API AssetData
        {
        public:
            AssetData( const std::string& name )
                : m_name( name )
            {}

            virtual ~AssetData() {}

            inline virtual const std::string& getName() const
            {
                return m_name;
            }

        protected:
            std::string m_name;
        };

    } // namespace Asset
} // namespace Ra

#endif // RADIUMENGINE_GEOMETRY_DATA_HPP
