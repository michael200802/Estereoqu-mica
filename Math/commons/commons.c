#include "commons.h"

inline integer_t get_gcd(integer_t num1, integer_t num2)
{
    integer_t remainder;
    do
    {
        remainder = num1%num2;
        num1 = num2;
        num2 = remainder;
    } while (remainder != 0);

    num1 *= (num1 < 0 ? -1 : 1);

    return num1;
}

inline integer_t get_lcm(integer_t num1, integer_t num2)
{
    integer_t result;
    result = (num1*num2)/get_gcd(num1,num2);
    result *= (result < 0 ? -1 : 1);
    return result;
}