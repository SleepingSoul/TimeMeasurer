#ifndef MEASUREEXECTIME_H
#define MEASUREEXECTIME_H

//this lib uses C++14 STL lib's

#if __cplusplus < 201402L
#error This library needs at least C++14
#endif

#include <chrono>
#include <future>
#include <thread>

enum Ratio {nano_seconds, micro_seconds, milli_seconds, centi_seconds, deci_seconds, seconds};

template <Ratio ratio>
class TimeMeasurer
{
    //class is static. You shouldn't create instances of this class to
    //measure time.
public:
    TimeMeasurer() = delete;

    //this constexpr function's use is to set up
    //ratio template for "std::ratio <>"

    static constexpr unsigned long long _ratio_to_ll(const Ratio r)
    {
        switch(r) {
        case nano_seconds:  return 1000000000ll;
        case micro_seconds: return 1000000ll;
        case milli_seconds: return 1000ll;
        case centi_seconds: return 100ll;
        case deci_seconds:  return 10ll;
        case seconds:       return 1ll;
        }
    }

    //function's template counts execution time of Callable object, passing "args".
    //returns value depended on ratio template argument.

    template <class Callable, class ...Args>
    static double measure_exec_time(Callable f, Args... args)
    {
        auto start = std::chrono::high_resolution_clock::now();
        f(args...);
        auto stop = std::chrono::high_resolution_clock::now();
        return std::chrono::duration <double, std::ratio <1, _ratio_to_ll(ratio)>>(stop - start).count();
    }

    //function's template starts new thread, which will execute Callable object, passing "args".
    //returns "std::future <double>" object which will store time value depended on ratio template argument.

    template <class Callable, class ...Args>
    static std::future <double> async_measure_exec_time(Callable f, Args... args)
    {
        return std::async(std::launch::async, [f, args...]
        {
            auto start = std::chrono::high_resolution_clock::now();
            f(args...);
            auto stop = std::chrono::high_resolution_clock::now();
            return std::chrono::duration <double, std::ratio <1, _ratio_to_ll(ratio)>>(stop - start).count();
        });
    }
};

#endif // MEASUREEXECTIME_H
