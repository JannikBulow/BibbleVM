// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_UTIL_TIME_MANAGER_H
#define BIBBLEVM_UTIL_TIME_MANAGER_H 1

#include <chrono>

namespace bibblevm {
    template<class T>
    concept Clock = std::chrono::is_clock_v<T>;

    using namespace std::chrono_literals;

    using Nanoseconds = std::chrono::nanoseconds;
    using Microseconds = std::chrono::microseconds;
    using Milliseconds = std::chrono::milliseconds;
    using Seconds = std::chrono::seconds;
    using Minutes = std::chrono::minutes;
    using Hours = std::chrono::hours;
    using Days = std::chrono::days;
    using Weeks = std::chrono::weeks;
    using Months = std::chrono::months;
    using Years = std::chrono::years;

    template<Clock Clk = std::chrono::steady_clock>
    class TimeManager {
    public:
        using Clock = Clk;
        using Duration = typename Clock::duration;
        using Rep = typename Clock::rep;
        using Period = typename Clock::period;
        using TimePoint = typename Clock::time_point;

        TimeManager() : mStart(Clock::now()) {}

        TimePoint start() const { return mStart; }

        template<class T = TimePoint>
        T now() const {
            if constexpr (std::is_same_v<T, TimePoint>) return TimePoint(Clock::now() - mStart);
            else return std::chrono::duration_cast<T>(Clock::now() - mStart);
        }

        template<class T = Nanoseconds>
        bool hasPassed(TimePoint prev, T duration) const {
            return (now() - prev) >= std::chrono::duration_cast<Duration>(duration);
        }

    private:
         TimePoint mStart;
    };
}

#endif // BIBBLEVM_UTIL_TIME_MANAGER_H
