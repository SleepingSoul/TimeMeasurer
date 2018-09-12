#ifndef MEASUREEXECTIME_H
#define MEASUREEXECTIME_H

//this lib uses C++14 STL lib's

#ifdef _WIN32
#include <windows.h>
#endif

#include <chrono>
#include <future>
#include <thread>

enum Ratio {nanoseconds, microseconds, milliseconds, seconds};

//this constexpr function's use is to set up
//ratio template for "std::ratio <>"
static constexpr unsigned long long _ratio_to_ll(const Ratio r)
{
    switch(r) {
    case nanoseconds:  return 1000000000ll;
    case microseconds: return 1000000ll;
    case milliseconds: return 1000ll;
    case seconds:      return 1ll;
    }
}

template <Ratio ratio>
class TimeMeasurer
{
    //class is static. You shouldn't create instances of this class to
    //measure time.
public:
    TimeMeasurer() = delete;

    //function's template counts execution time of Callable object, passing "args".
    //returns value depended on ratio template argument.

    template <class Callable, class ...Args>
    static long double measure_exec_time(Callable f, Args... args)
    {
        auto start = std::chrono::high_resolution_clock::now();
        f(args...);
        auto stop = std::chrono::high_resolution_clock::now();
        return std::chrono::duration <long double, std::ratio <1, _ratio_to_ll(ratio)>>(stop - start).count();
    }

    //function's template starts new thread, which will execute Callable object, passing "args".
    //returns "std::future <double>" object which will store time value depended on ratio template argument.

    template <class Callable, class ...Args>
    static std::future <long double> async_measure_exec_time(Callable f, Args... args)
    {
        return std::async(std::launch::async, [f, args...]
        {
            auto start = std::chrono::high_resolution_clock::now();
            f(args...);
            auto stop = std::chrono::high_resolution_clock::now();
            return std::chrono::duration <long double, std::ratio <1, _ratio_to_ll(ratio)>>(stop - start).count();
        });
    }
};

#ifdef _WIN32
namespace TickMeasurer
{
    template <class Callable, class ...Args>
    static uint64_t measure_exec_ticks(Callable f, Args ...args)
    {
        LARGE_INTEGER start, stop;
        QueryPerformanceCounter(&start);
        f(args...);
        QueryPerformanceCounter(&stop);
        return stop.QuadPart - start.QuadPart;
    }
}
#endif

//this class uses it's existance time in code to measure time
template <Ratio ratio>
class TimeGuard
{
public:
	TimeGuard(long double &dest) noexcept
		: start(std::chrono::high_resolution_clock::now()), out(dest)
	{}
	TimeGuard(const TimeGuard &) = delete;
	TimeGuard(TimeGuard &&) = delete;
	TimeGuard& operator =(const TimeGuard &) = delete;
	TimeGuard& operator =(TimeGuard&&) = delete;
	
	~TimeGuard()
	{
		auto stop = std::chrono::high_resolution_clock::now();
		out = std::chrono::duration <long double, std::ratio <1, _ratio_to_ll(ratio)>>(stop - start).count();
	}
	
private:
	std::chrono::time_point <std::chrono::high_resolution_clock> start;
	long double &out;
};

#ifdef _WIN32
class TickGuard
{
public:
	TickGuard(uint64_t &dest) noexcept
		: out(dest)
	{
		QueryPerformanceCounter(&start);
	}
	TickGuard(const TickGuard &) = delete;
	TickGuard(TickGuard &&) = delete;
	TickGuard& operator =(const TickGuard &) = delete;
	TickGuard& operator =(TickGuard&&) = delete;
	
	~TickGuard()
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

#endif // MEASUREEXECTIME_H
