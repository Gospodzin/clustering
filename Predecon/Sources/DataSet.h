#pragma once
#include <memory>
#include "Point.h"
#include "measures.h"
#include "logging.h"

struct DataSet
{
	std::vector<Point>* data;
	measures::Measure measure;
	measures::AttrsMeasure attrsMeasure;
	measures::MeasureId measureId;

	DataSet(std::vector<Point>* data, measures::MeasureId measureId) : data(data), measureId(measureId), 
		measure(measures::getMeasure(measureId)), attrsMeasure(measures::getAttrsMeasure(measureId)) {}

	int dimensions() const { return data->empty() ? -1 : data->front().size(); }
	int size() const { return data->size(); }
	std::vector<Point>::iterator begin() const { return data->begin(); }
	std::vector<Point>::iterator end() const { return data->end(); }
	Point& operator[](const int& n) const { return data->at(n); }
	void cleanUp() { for (Point& p : *data) p.cid = NONE; }

protected:
	double distance(const Point& p1, const Point& p2, const std::vector<int>& attrs) const { 
		return attrs.size() == 0 ? measure(p1, p2) : attrsMeasure(p1, p2, attrs); 
	}
};

