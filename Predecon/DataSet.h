#pragma once
#include <vector>
#include "Point.h"
#include "measures.h"

struct DataSet
{
	std::shared_ptr<std::vector<Point> > data;

	DataSet(std::vector<Point>* data) : data(std::shared_ptr<std::vector<Point> >(data)) {}

	int size() {
		return data->size();
	}

	std::vector<Point>::iterator begin() {
		return data->begin();
	}

	std::vector<Point>::iterator end() {
		return data->end();
	}

	Point& operator[](const int& n) {
		return data->at(n);
	}
	
	std::vector<Point*> regionQuery(const Point& target, const double& eps, measures::Measure measure);
};

