#pragma once
#include <chrono>
#include <stack>
#include <iostream>
#include <string>

namespace logging {
    struct Logger {
        virtual void log(std::string val) {
            std::cout << val << std::endl;
        }
    };

    struct LOG {
        static std::stack<clock_t> ticks;
        static Logger* out;
    };
}

#define LOG(m) \
    logging::LOG::out->log(m);

#define TS() \
    logging::LOG::ticks.push(clock());

#define TP() \
    logging::LOG::out->log(std::to_string(double(clock() - logging::LOG::ticks.top())/ CLOCKS_PER_SEC)); logging::LOG::ticks.pop();

