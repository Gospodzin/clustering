#pragma once
#include <memory>
#include "Point.h"
#include "measures.h"
#include "logging.h"
#include "referenceSelectors.h"

struct DataSet
{
	struct Params {
		Params() {}
		Params(measures::MeasureId measureId) : measureId(measureId) {}
		Params(measures::MeasureId measureId, referenceSelectors::ReferenceSelector referenceSelector) : measureId(measureId), referenceSelector(referenceSelector) {}
		Params(measures::MeasureId measureId, Point reference) : measureId(measureId), reference(reference) {}
		Params(measures::MeasureId measureId, int n) : measureId(measureId), n(n) {}
		Params(measures::MeasureId measureId, double eps, int n) : measureId(measureId), eps(eps), n(n) {}
		Params(measures::MeasureId measureId, std::vector<int> dims) : measureId(measureId), dims(dims) {}

		measures::MeasureId measureId;
		double eps;
		int n;
		referenceSelectors::ReferenceSelector referenceSelector;
		Point reference;
		std::vector<int> attrs = {};
		std::vector<int> dims = {};
	};

	std::vector<Point>* data;
	measures::Measure measure;
	measures::AttrsMeasure attrsMeasure;
	measures::MeasureId measureId;

	DataSet(std::vector<Point>* data, Params params) : data(data), measureId(params.measureId), 
        measure(measures::getMeasure(params.measureId)), attrsMeasure(measures::getAttrsMeasure(params.measureId)) {}

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

