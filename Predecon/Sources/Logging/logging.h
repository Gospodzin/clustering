#pragma once
#include <chrono>
#include <stack>

struct LOG {
	static std::stack<clock_t> ticks;
};

#define LOG(m) \
	printf("%s\n", m);

#define TS() \
	LOG::ticks.push(clock());

#define TP() \
	printf("%f\n", double(clock() - LOG::ticks.top()) / CLOCKS_PER_SEC); LOG::ticks.pop();

