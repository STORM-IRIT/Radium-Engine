#include <Engine/Engine.hpp>

#include <cstdio>
#include <Core/Math/Vector.hpp>

namespace Ra
{

void Engine::sayHello()
{
    printf("Hello from Radium Engine !\n");

    Vector3 v1(1, 2, 3);
    Vector3 v2(4, 5, 6);

    printf("(%.3f %.3f %.3f) . (%.3f %.3f %.3f) = %.3f\n",
           v1.x(), v1.y(), v1.z(),
           v2.x(), v2.y(), v2.z(),
           v1.dot(v2));

}

} // namespace RadiumEngine
