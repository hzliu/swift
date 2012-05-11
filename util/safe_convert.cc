#include "util/safe_convert.h"

#include <limits.h>
#include <stdlib.h>

#define SAFE_CONVERT(func, type)                \
    bool safe_##func(const char *str, type *v)  \
    {                                           \
        if(!str || *str == '\0')                \
            return false;                       \
        char *tmp = NULL;                       \
        type vv = func(str, &tmp);              \
        if(*tmp != '\0')                        \
            return false;                       \
        *v = vv;                                \
        return true;                            \
    }

SAFE_CONVERT(strtof, float)
SAFE_CONVERT(strtod, double)
SAFE_CONVERT(strtold, long double)
#undef SAFE_CONVERT

#define SAFE_CONVERT(func, type)                \
    bool safe_##func(const char *str, type *v)  \
    {                                           \
        if(!str || *str == '\0')                \
            return false;                       \
        char *tmp = NULL;                       \
        type vv = func(str, &tmp, 10);          \
        if(*tmp != '\0')                        \
            return false;                       \
        *v = vv;                                \
        return true;                            \
    }

SAFE_CONVERT(strtoul, unsigned long)
SAFE_CONVERT(strtol, long)
SAFE_CONVERT(strtoll, long long)
SAFE_CONVERT(strtoull, unsigned long long)

#undef SAFE_CONVERT

template<typename T>
bool safe_convert_bound(const char *str, T *v, long max, long min)
{
    long vv;
    if(!safe_strtol(str, &vv))
        return false;
    if(vv > max || vv < min)
        return false;
    *v = static_cast<T>(vv);
    return true;
}

bool safe_strtoi(const char *str, int *v)
{
    return safe_convert_bound(str, v, INT_MAX, INT_MIN);
}

bool safe_strtos(const char *str, short *v)
{
    return safe_convert_bound(str, v, SHRT_MAX, SHRT_MIN);
}

bool safe_strtous(const char *str, unsigned short *v)
{
    return safe_convert_bound(str, v, USHRT_MAX, 0);
}

bool safe_strtoui(const char *str, unsigned int *v)
{
    return safe_convert_bound(str, v, UINT_MAX, 0);
}


