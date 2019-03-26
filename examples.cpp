#include <iostream>
#include <memory>
#include "time_measurer.hpp"


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
    //We measure lambda execution time.
    std::cout << measure::executeAndMeasureTime <measure::Ratio::seconds>(
        []{sleep_for(std::chrono::seconds(3));}
    ) << '\n';

    //We measure "do something" execution time, passing lvalue args. a, b will be copied by default
    int a = 255, b = -1;
    std::cout << measure::executeAndMeasureTime <measure::Ratio::milliseconds>(do_something, a, b) << '\n';
    //after: a = 255, b = -1

    //We measure "do something" execution time, passing reference wrappers. They will be pased as references this time.
    std::cout << measure::executeAndMeasureTime <measure::Ratio::milliseconds>(do_something, std::ref(a), std::ref(b)) << '\n';
    //after: a = 256. b = 0

    //Now we will measure ticks for small function (TimeMeasurer will probably return 0)
    std::cout << measure::executeAndMeasureTicks(small_operation);

    return 0;
}
