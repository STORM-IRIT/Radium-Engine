#ifndef RADIUMENGINE_ENTITYMANAGER_HPP
#define RADIUMENGINE_ENTITYMANAGER_HPP

#include <Core/Utils/Singleton.hpp>

namespace Ra
{

class EntityManager : public Singleton<EntityManager>
{
    friend class Singleton<EntityManager>;

public:

private:
    /// CONSTRUCTOR
    EntityManager() {}

    /// DESTRUCTOR
    virtual ~EntityManager();
};

} // namespace Ra

#endif // RADIUMENGINE_ENTITYMANAGER_HPP
