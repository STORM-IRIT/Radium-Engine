#include <iostream>
#include <Core/Math/LinearAlgebra.hpp>
int main()
{
    Ra::Core::Vector3 a (1,2,3);
    std::cout<<"Hello tests "<<a.transpose()<<std::endl;
    return 0;
}

