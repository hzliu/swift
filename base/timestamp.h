#ifndef TIMESTAMP_H__
#define TIMESTAMP_H__

#include <stdint.h>
#include <string>

class timestamp
{
public:
    timestamp() : m_useconds(0) {}
    explicit timestamp(int64_t usecondsSinceEpoch) :
        m_useconds(usecondsSinceEpoch) {}

    bool valid() const { return m_useconds > 0; }
    static timestamp now();
    std::string to_string() const;
    std::string to_date_string() const;

    int64_t useconds() const { return m_useconds; }

    static const int usecondsPerSecond = 1e6;

private:
    int64_t m_useconds; //micro seconds since epoch
};

inline bool operator <(const timestamp &lhs, const timestamp &rhs)
{
    return lhs.useconds() < rhs.useconds();
}

inline bool operator ==(const timestamp &lhs, const timestamp &rhs)
{
    return lhs.useconds() == rhs.useconds();
}

int operator -(const timestamp &lhs, const timestamp &rhs)
{
    return static_cast<int>(lhs.useconds() - rhs.useconds());
}


#endif
