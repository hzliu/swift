#include <stdio.h>
#include <unistd.h>
#include <swift/net/timer_queue.h>

using namespace swift;
using namespace swift::net;

void PrintTime(Timestamp when)
{
    Timestamp now = Timestamp::Now();
    printf("should run at %s, now = %s, diff = %d\n",
            when.ToString().c_str(), now.ToString().c_str(),
            static_cast<int>(
                now.MicrosecondsSinceEpoch() - when.MicrosecondsSinceEpoch())
          );
}

int main()
{
    TimerQueue queue;

    for(int i = 0; i < 10000; ++i)
    {
        queue.ScheduleTimer(Timestamp::MillisecondsLater(i),
                std::tr1::bind(PrintTime, Timestamp::MillisecondsLater(i)));
    }

    while(true)
    {
        printf("before wheel.RunUntil, now = %s\n", Timestamp::Now().ToString().c_str());
        queue.RunUntil(Timestamp::Now());
        printf("after wheel.RunUntil, now = %s\n", Timestamp::Now().ToString().c_str());

        int to_sleep = static_cast<int>(
                queue.NextTimeout().MicrosecondsSinceEpoch() -
                Timestamp::MicrosecondsUntilNow());
        printf("will sleep for %d microseconds\n", to_sleep);
        if(to_sleep > 0)
            usleep(to_sleep);
    }

    return 0;
}

