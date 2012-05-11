#include <swift/base/timestamp.h>

#include <sys/time.h>
#include <ctime>
#include <cstdio>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#undef __STDC_FORMAT_MACROS

namespace swift
{

std::string Timestamp::ToString() const
{
    int64_t seconds = microseconds_ / kMicrosecondsPerSecond;
    int64_t microseconds = microseconds_ % kMicrosecondsPerSecond;

    char buf[32];
    snprintf(buf, sizeof(buf) - 1,
            "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
    return buf;
}

std::string Timestamp::ToDateTimeString() const
{
    time_t seconds =
        static_cast<time_t>(microseconds_ / kMicrosecondsPerSecond);
    int microseconds =
        static_cast<int>(microseconds_ % kMicrosecondsPerSecond);
    struct tm tm_time;
    gmtime_r(&seconds, &tm_time);

    char buf[32];
    snprintf(buf, sizeof(buf), "%4d-%02d-%02d %02d:%02d:%02d.%06d",
            tm_time.tm_year + 1900, tm_time.tm_mon + 1,
            tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min,
            tm_time.tm_sec, microseconds);
    return buf;
}

Timestamp Timestamp::Now()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t seconds = tv.tv_sec;
    return Timestamp(seconds * kMicrosecondsPerSecond + tv.tv_usec);
}

}
