#include <sys/time.h>
#include <ctime>
#include <cstdio>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#undef __STDC_FORMAT_MACROS


#include "timestamp.h"

std::string timestamp::to_string() const
{
    char buf[32];
    int64_t seconds = m_useconds / usecondsPerSecond;
    int64_t microseconds = m_useconds % usecondsPerSecond;
    snprintf(buf, sizeof(buf) - 1, 
            "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
    return buf;
}

std::string timestamp::to_date_string() const
{
    char buf[32];
    time_t seconds =
        static_cast<time_t>(m_useconds / usecondsPerSecond);
    int microseconds =
        static_cast<int>(m_useconds % usecondsPerSecond);
    struct tm tm_time;
    gmtime_r(&seconds, &tm_time);

    snprintf(buf, sizeof(buf), "%4d-%02d-%02d %02d:%02d:%02d.%06d",
            tm_time.tm_year + 1900, tm_time.tm_mon + 1, 
            tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min,
            tm_time.tm_sec, microseconds);
    return buf;
}

timestamp timestamp::now()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t seconds = tv.tv_sec;
    return timestamp(seconds * usecondsPerSecond + tv.tv_usec);
}


