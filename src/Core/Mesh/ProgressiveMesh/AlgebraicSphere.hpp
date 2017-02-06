#ifndef ALGEBRAIC_SPHERE_H
#define ALGEBRAIC_SPHERE_H

#include <Patate/common/defines.h>

template < class DataPoint, class _WFunctor, typename T>
class APSSInterpolation : public T
{
private:
    typedef T Base;

protected:
    enum
    {
        Check = Base::PROVIDES_ALGEBRAIC_SPHERE,
        PROVIDES_APSS_INTERPOLATION
    };

public:
    typedef typename Base::Scalar     Scalar;     /*!< \brief Inherited scalar type*/
    typedef typename Base::VectorType VectorType; /*!< \brief Inherited vector type*/
    typedef typename Base::WFunctor   WFunctor;   /*!< \brief Weight Function*/

    /*! \brief Default constructor */
    MULTIARCH inline APSSInterpolation()
        : Base(){}

    //MULTIARCH inline void combine()

};

#endif
