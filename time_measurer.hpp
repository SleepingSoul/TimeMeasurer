#ifndef MEASUREEXECTIME_H
#define MEASUREEXECTIME_H

//this lib uses C++14 STL lib's

#ifdef _WIN32
#include <windows.h>
#endif

#include <chrono>
#include <future>
#include <thread>

namespace measure
{
    enum class Ratio
    {
        nanoseconds,
        microseconds,
        milliseconds,
        seconds
    };

    using DurationType = long double;
    using TicksCountType = std::uint64_t;

    inline constexpr unsigned long long convertRatio(Ratio r)
    {
        switch(r)
        {
            case Ratio::nanoseconds:  return 1000000000ll;
            case Ratio::microseconds: return 1000000ll;
            case Ratio::milliseconds: return 1000ll;
            case Ratio::seconds:      return 1ll;
        }
    }

    template <Ratio ratio, class Callable, class ...Args>
    static DurationType executeAndMeasureTime(Callable f, Args&&... args)
    {
        auto start = std::chrono::high_resolution_clock::now();
        f(std::forward <Args>(args)...);
        auto stop = std::chrono::high_resolution_clock::now();
        return std::chrono::duration <DurationType, std::ratio <1, convertRatio(ratio)>>(stop - start).count();
    }

    #ifdef _WIN32
    template <class Callable, class ...Args>
    static TicksCountType executeAndMeasureTicks(Callable f, Args&&... args)
    {
        LARGE_INTEGER start, stop;
        QueryPerformanceCounter(&start);
        f(std::forward <Args>(args)...);
        QueryPerformanceCounter(&stop);
        return stop.QuadPart - start.QuadPart;
    }
    #endif

    template <Ratio ratio>
    class ThisObjectLifetime
    {
    public:
        ThisObjectLifetime(DurationType& dest) noexcept
            : start(std::chrono::high_resolution_clock::now())
            , out(dest)
        {}
        ThisObjectLifetime(ThisObjectLifetime const&) = delete;
        ThisObjectLifetime(ThisObjectLifetime&&) = delete;
        ThisObjectLifetime& operator =(ThisObjectLifetime const&) = delete;
        ThisObjectLifetime& operator =(ThisObjectLifetime&&) = delete;
        
        ~ThisObjectLifetime()
        {
            auto stop = std::chrono::high_resolution_clock::now();
            out = std::chrono::duration <DurationType, std::ratio <1, convertRatio(ratio)>>(stop - start).count();
        }
        
    private:
        std::chrono::time_point <std::chrono::high_resolution_clock> start;
        DurationType &out;
    };

#ifdef _WIN32
    class ThisObjectLifetimeTicks
    {
    public:
        ThisObjectLifetimeTicks(uint64_t& dest) noexcept
            : out(dest)
        {
            QueryPerformanceCounter(&start);
        }
        ThisObjectLifetimeTicks(ThisObjectLifetimeTicks const&) = delete;
        ThisObjectLifetimeTicks(ThisObjectLifetimeTicks&&) = delete;
        ThisObjectLifetimeTicks& operator =(ThisObjectLifetimeTicks const&) = delete;
        ThisObjectLifetimeTicks& operator =(ThisObjectLifetimeTicks&&) = delete;
        
        ~ThisObjectLifetimeTicks()
        {
            LARGE_INTEGER stop;
            QueryPerformanceCounter(&stop);
            out = stop.QuadPart - start.QuadPart;
        }
        
    private:
        LARGE_INTEGER start;
        uint64_t &out;
    };
#endif
}

#endif // MEASUREEXECTIME_H
