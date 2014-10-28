#pragma once
#include <vector>
#include "Point.h"

struct DataSet : std::vector<Point>
{
	DataSet(int n){ reserve(n); }
};

