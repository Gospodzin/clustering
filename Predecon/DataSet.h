#pragma once
#include <vector>
#include "Point.h"
#include "measures.h"

struct DataSet
{
	std::vector<Point>::iterator begin();
	std::vector<Point>::iterator end();
	Point& operator[](const int& n);
	std::vector<Point*> regionQuery(const int& pointOrderId, const double& eps, measures::Measure measure);
};

