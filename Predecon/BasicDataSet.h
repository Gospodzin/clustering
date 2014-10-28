#pragma once
#include "DataSet.h"

struct BasicDataSet : DataSet
{
	BasicDataSet(std::vector<Point>* data) : DataSet(data) {}

	std::vector<Point*> regionQuery(const Point& target, const double& eps, measures::Measure measure) {
		std::vector<Point*> neighbours;
		for (Point& p : *data)
			if (measure(target, p) <= eps)
				neighbours.push_back(&p);
		return neighbours;
	}
};

