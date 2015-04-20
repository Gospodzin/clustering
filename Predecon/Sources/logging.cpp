#include "logging.h"

std::stack<clock_t> logging::LOG::ticks = std::stack<clock_t>();
char logging::LOG::indent = '>';
logging::Logger* logging::LOG::out = new logging::Logger();
