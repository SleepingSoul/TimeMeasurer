#include <iostream>
#include <memory>
#include "headers/TimeMeasurer.hpp"

using std::this_thread::sleep_for;

void do_something(int &a, int &b)
{
    sleep_for(std::chrono::seconds(1));
    ++a;
    ++b;
}

void small_operation()
{
    auto ptr = std::make_unique <double>();
}

int main()
{
    //We measure lambda execution time. It will return a little more than 3 seconds, because of lambda overheads
    std::cout << TimeMeasurer <seconds>::measure_exec_time
                 ([]
                  {
                      sleep_for(std::chrono::seconds(3));
                  }
                 ) << '\n';

    //We measure "do something" execution time, passing lvalue args. They will be pased not as references!
    int a = 255,
        b = -1;
    std::cout << TimeMeasurer <milliseconds>::measure_exec_time(do_something, a, b) << '\n';
    //after: a = 255, b = -1

    //We measure "do something" execution time, passing reference wrappers. They will be pased as references this time.
    std::cout << TimeMeasurer <milliseconds>::measure_exec_time(do_something, std::ref(a), std::ref(b)) << '\n';
    //after: a = 256. b = 0

    //Now, we will run "do_something" asynchronous.
    auto f_res = TimeMeasurer <milliseconds>::async_measure_exec_time(do_something, std::ref(a), std::ref(b));
    //...some code.
    std::cout << f_res.get() << '\n';
    //after: a = 257, b = 1

    //Now we will measure ticks for small function (TimeMeasurer will probably return 0)
    std::cout << TickMeasurer::measure_exec_ticks(small_operation);

    return 0;
}
