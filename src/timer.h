#ifndef TIDE_TIMER_H
#define TIDE_TIMER_H

#include <chrono>
#include <fmt/chrono.h>

template <typename D>
class Timer
{
    public:
        Timer<D>()
        {
            start = std::chrono::high_resolution_clock::now();
        }
        ~Timer<D>()
        {
            end = std::chrono::high_resolution_clock::now();
            duration = end - start;

            // fmt::print("Timer took: {}\n", std::chrono::duration_cast<D>(duration));
        }
        D LogTime()
        {
            auto log = std::chrono::high_resolution_clock::now();
            auto dur = std::chrono::duration_cast<D>(log - start);
            return dur;
        }
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
        std::chrono::duration<float> duration;
};

#endif // TIDE_TIMER_H