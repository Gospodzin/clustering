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
		static char indent;
    };
}

#define LOG(m) \
    logging::LOG::out->log(std::string(logging::LOG::ticks.size(), logging::LOG::indent) + m);

#define TS(m) \
	logging::LOG::out->log(std::string(logging::LOG::ticks.size(), logging::LOG::indent) + m); \
    logging::LOG::ticks.push(clock()); 

#define TP(m) \
    logging::LOG::out->log(std::string(logging::LOG::ticks.size() - 1, logging::LOG::indent) + std::string(m) + ": " + std::to_string(double(clock() - logging::LOG::ticks.top())/ CLOCKS_PER_SEC)); \
    logging::LOG::ticks.pop();
