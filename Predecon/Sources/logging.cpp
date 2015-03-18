#include "logging.h"

std::stack<clock_t> logging::LOG::ticks = std::stack<clock_t>();
logging::Logger* logging::LOG::out = new logging::Logger();
