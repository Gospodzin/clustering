#pragma once
#include <vector>
#include "Point.h"

struct BasicDataSet : std::vector<Point>
{
	BasicDataSet(int n) { reserve(n); }
};

