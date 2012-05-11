#ifndef SWIFT_BASE_TIMESTAMP_H__
#define SWIFT_BASE_TIMESTAMP_H__

#include <stdint.h>
#include <string>

namespace swift
{

class Timestamp
{
public:
    ///default constructor: an invalid timestamp
    Timestamp() : microseconds_(0) {}

    explicit Timestamp(int64_t microseconds_since_epoch) :
        microseconds_(microseconds_since_epoch) {}

    bool Valid() const { return microseconds_ > 0; }

    static Timestamp Now();

    std::string ToString() const;

    std::string ToDateTimeString() const;

    int64_t MicrosecondsSinceEpoch() const { return microseconds_; }

    static const int kMicrosecondsPerSecond = 1e6;

private:
    int64_t microseconds_; //micro seconds since epoch
};

inline bool operator <(const Timestamp lhs, const Timestamp rhs)
{
    return lhs.MicrosecondsSinceEpoch() < rhs.MicrosecondsSinceEpoch();
}

inline bool operator ==(const Timestamp lhs, const Timestamp rhs)
{
    return lhs.MicrosecondsSinceEpoch() == rhs.MicrosecondsSinceEpoch();
}

inline double TimeDifference(Timestamp high, Timestamp low)
{
    int64_t diff = high.MicrosecondsSinceEpoch() - low.MicrosecondsSinceEpoch();
    return static_cast<double>(diff) / Timestamp::kMicrosecondsPerSecond;
}

}
#endif
