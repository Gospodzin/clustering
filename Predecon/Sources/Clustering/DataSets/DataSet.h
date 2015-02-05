#pragma once
#include <memory>
#include "Clustering/Point.h"
#include "Clustering/measures.h"
#include "Logging/logging.h"

struct DataSet
{
	std::vector<Point>* data;
	measures::Measure measure;
	measures::MeasureId measureId;

	DataSet(std::vector<Point>* data, measures::MeasureId measureId) 
		: data(data), measureId(measureId), measure(measures::getMeasure(measureId)) {}

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

