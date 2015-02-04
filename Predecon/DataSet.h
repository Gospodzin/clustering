#pragma once
#include <memory>
#include "Point.h"
#include "measures.h"
#include "logging.h"

struct DataSet
{
	std::vector<Point>* data;
	measures::Measure measure;

	DataSet(std::vector<Point>* data, measures::Measure measure) 
		: data(data), measure(measure) {}

	int size() {
		return data->size();
	}

	int dimensions() {
		if (data->empty())
			return -1;
		return data->front().size();
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
	
	std::vector<Point*> regionQuery(const Point& target, const double& eps);
};

