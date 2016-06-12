#pragma once
#include "Point.h"

namespace measures{
	typedef  double(*Measure)(const Point&, const Point&);
	typedef  double(*PrefMeasure)(const Point&, const Point&, const std::vector<double>& prefVector);
	typedef  double(*AttrsMeasure)(const Point&, const Point&, const std::vector<int>& attrs);

	enum MeasureId {
		Euclidean,
		Manhattan,
		CosDist
	};

	double euclideanDistance(const Point& p1, const Point& p2);

	double euclideanDistance(const Point& p1, const Point& p2, const std::vector<double>& prefVector);

	double euclideanDistance(const Point& p1, const Point& p2, const std::vector<int>& attrs);

	double manhattanDistance(const Point& p1, const Point& p2);

	double manhattanDistance(const Point& p1, const Point& p2, const std::vector<double>& prefVector);

	double manhattanDistance(const Point& p1, const Point& p2, const std::vector<int>& attrs);

	Measure getMeasure(MeasureId measureId);
	PrefMeasure getPrefMeasure(MeasureId measureId);
	AttrsMeasure getAttrsMeasure(MeasureId measureId);
}
