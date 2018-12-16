#include <Core/Utils/Attribs.hpp>
#include <Core/Log/Log.hpp>

namespace Ra {
namespace Core {
namespace Utils {

void AttribManager::clear() {
    for ( auto attr : m_attribs )
    {
        delete attr;
    }
    m_attribs.clear();
    m_attribsIndex.clear();
}


void AttribManager::copyAllAttributes(const AttribManager& m) {
    for (const auto& attr : m.m_attribs)
    {
        if (attr == nullptr)
            continue;
        if (attr->isFloat())
        {
            auto h = addAttrib<float>(attr->getName());
            getAttrib(h).data() = static_cast<Attrib<float>*>(attr)->data();
        }
        else if (attr->isVec2())
        {
            auto h = addAttrib<Vector2>(attr->getName());
            getAttrib(h).data() = static_cast<Attrib<Vector2>*>(attr)->data();
        }
        else if (attr->isVec3())
        {
            auto h = addAttrib<Vector3>(attr->getName());
            getAttrib(h).data() = static_cast<Attrib<Vector3>*>(attr)->data();
        }
        else if (attr->isVec4())
        {
            auto h = addAttrib<Vector4>(attr->getName());
            getAttrib(h).data() = static_cast<Attrib<Vector4>*>(attr)->data();
        }
        else
            LOG(logWARNING)
            << "Warning, mesh attribute " << attr->getName()
            << " type is not supported (only float, vec2, vec3 nor vec4 are supported)";
    }
}

bool AttribManager::hasSameAttribs( const AttribManager& other ) {
    // one way
    for ( const auto& attr : m_attribsIndex )
    {
        if ( other.m_attribsIndex.find( attr.first ) == other.m_attribsIndex.cend() )
        {
            return false;
        }
    }
    // the other way
    for ( const auto& attr : other.m_attribsIndex )
    {
        if ( m_attribsIndex.find( attr.first ) == m_attribsIndex.cend() )
        {
            return false;
        }
    }
    return true;
}

} // namespace Utils
} // namespace Core
} // namespace Ra

