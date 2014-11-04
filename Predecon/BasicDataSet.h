#pragma once
#include "DataSet.h"

struct BasicDataSet : DataSet
{
	BasicDataSet(std::vector<Point>* data, measures::Measure measure) : DataSet(data, measure) {}

	std::vector<Point*> regionQuery(const Point& target, const double& eps) {
		std::vector<Point*> neighbours;
		for (Point& p : *data)
			if (measure(target, p) <= eps)
				neighbours.push_back(&p);
		return neighbours;
	}
};

