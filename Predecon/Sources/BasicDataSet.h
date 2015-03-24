#pragma once
#include "DataSet.h"

struct BasicDataSet : DataSet
{
	BasicDataSet(std::vector<Point>* data, measures::MeasureId measure, ...) : DataSet(data, measure) {}

	std::vector<Point*> regionQuery(const Point& target, const double& eps, const std::vector<int>& attrs = {}) {
		std::vector<Point*> neighbours;
		for (Point& p : *data)
			if (distance(target, p, attrs) <= eps)
				neighbours.push_back(&p);

		return neighbours;
	}
};
