#include <Core/Math/Math.hpp>

namespace Ra
{
    namespace Core
    {
        namespace Math
        {
            inline Scalar toRadians ( Scalar a )
            {
                Scalar result = a * Pi / 180.0;
                return result;
            }

            inline Scalar toDegrees ( Scalar a )
            {
                Scalar result = a * 180 / Pi;
                return result;
            }

            template<class T> static inline T ipow(T x, int p)
            {
                assert(p >= 0);
                if (p == 0) return 1;
                if (p == 1) return x;
                return x *  ipow(x, p-1);
            }
            
            template <int n> inline float ipow(float a) 
            {
                const float b = ipow<n/2>(a);
                return (n & 1) ? (a * b * b) : (b * b);
            }

            template <> inline float ipow<1>(float a){ return a;   }
            template <> inline float ipow<0>(float  ){ return 1.f; }

            template <int n> inline int ipow(int a)
            {
                const int b = ipow<n/2>(a);
                return (n & 1) ? (a * b * b) : (b * b);
            }

            template <> inline int ipow<1>(int a){ return a; }
            template <> inline int ipow<0>(int  ){ return 1; }
            
            template <typename T>
            int sign(T val)
            {
                return (T(0) < val) - (val < T(0));
            }
            
            template <typename T>
            T clampf(T v, T min, T max)
            {
                if (max < v)
                    return max;
                if (min > v)
                    return min;
                return v;
            }
        }
    }
}
