// Author: Hongzhang Liu
// Date:

#ifndef SWIFT_UTIL_SAFE_CONVERT_H__
#define SWIFT_UTIL_SAFE_CONVERT_H__

/**
 * safe_* 只有当整个字符串是有效时，才返回true
 */

bool safe_strtof(const char *str, float *v);
bool safe_strtod(const char *str, double *v);
bool safe_strtold(const char *str, long double *v);
bool safe_strtoul(const char *str, unsigned long *v);
bool safe_strtol(const char *str, long *v);
bool safe_strtoi(const char *str, int *v);
bool safe_strtoui(const char *str, unsigned int *v);
bool safe_strtoul(const char *str, unsigned long *v);
bool safe_strtoull(const char *str, unsigned long long *v);
bool safe_strtos(const char *str, short *v);
bool safe_strtous(const char *str, unsigned short *v);

#endif

