#ifndef TIDE_ASSERT_H
#define TIDE_ASSERT_H

#include <fmt/core.h>
#include <fmt/chrono.h>
#include <ctime>
#include "io.h"

void _INTERNAL_TIDE_AssertLog(const char* expressionString, const char* file, int line);
void _INTERNAL_TIDE_AbortLog(const char* msg, const char* file, int line);

#ifdef TIDE_ASSERT_IMPL

void _INTERNAL_TIDE_AssertLog(const char* expressionString, const char* file, int line)
{
    auto curTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    WriteFile("assertion.log", fmt::format("{}TIDE Assertion '{}' failed @ {}:{}", std::ctime(&curTime), expressionString, file, line));
    fmt::print("TIDE Assertion '{}' failed @ {}:{}\n", expressionString, file, line);
    std::abort();
}

void _INTERNAL_TIDE_AbortLog(const char* msg, const char* file, int line)
{
    auto curTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    WriteFile("assertion.log", fmt::format("{}TIDE Abort '{}' @ {}:{}", std::ctime(&curTime), msg, file, line));
    fmt::print("TIDE Abort '{}' @ {}:{}", msg, file, line);
    std::abort();
}

#endif


#define TIDE_DEBUG

#ifdef NDEBUG
    #define TIDE_ASSERT(exp) ((void)0) 
#else
    #ifdef TIDE_DEBUG
        #define TIDE_ASSERT(exp) \
            if(!exp) \
                _INTERNAL_TIDE_AssertLog(#exp, __FILE__, __LINE__)
    #else
        #define TIDE_ASSERT(exp) ((void)0)
    #endif // TIDE_DEBUG
#endif // NDEBUG

#ifdef TIDE_DEBUG
    #define TIDE_ABORT(msg) _INTERNAL_TIDE_AbortLog(msg, __FILE__, __LINE__);
#else
    #define TIDE_ABORT(msg) ((void)0)
#endif // TIDE_DEBUG

#endif // TIDE_ASSERT_H