#pragma once
#include "Point.h"

namespace measures{
	typedef  double(*Measure)(const Point&, const Point&);
	typedef  double(*PrefMeasure)(const Point&, const Point&, std::vector<double>& prefVector);

	enum MeasureId {
		Euclidean,
		Manhattan
	};

	double euclideanDistance(const Point& p1, const Point& p2) {
		double res = 0;
		for (int i = 0; i < (int)(int)p1.size(); ++i) {
			double diff = p1[i] - p2[i];
			res += diff*diff;
		}
		return sqrt(res);
	}

	double euclideanDistance(const Point& p1, const Point& p2, std::vector<double>& prefVector) {
		double res = 0;
		for (int i = 0; i < (int)p1.size(); ++i) {
			double diff = p1[i] - p2[i];
			res += prefVector[i] * diff * diff;
		}
		return sqrt(res);
	}

	double euclideanDistance(const Point& p1, const Point& p2, const std::vector<int>& attrs) {
		double res = 0;
		for (int i = 0; i < (int)(int)attrs.size(); ++i) {
			double diff = p1[attrs[i]] - p2[attrs[i]];
			res += diff*diff;
		}
		return sqrt(res);
	}

	double manhattanDistance(const Point& p1, const Point& p2) {
		double res = 0;
		for (int i = 0; i < (int)p1.size(); ++i)
			res += std::abs(p1[i] - p2[i]);
		return res;
	}

	double manhattanDistance(const Point& p1, const Point& p2, std::vector<double>& prefVector) {
		double res = 0;
		for (int i = 0; i < (int)p1.size(); ++i)
			res += prefVector[i] * std::abs(p1[i] - p2[i]);
		return res;
	}

	Measure getMeasure(MeasureId measureId) {
		switch (measureId) {
		case Euclidean:
			return euclideanDistance;
		case Manhattan:
			return manhattanDistance;
		}
	}

	PrefMeasure getPrefMeasure(MeasureId measureId) {
		switch (measureId) {
		case Euclidean:
			return euclideanDistance;
		case Manhattan:
			return manhattanDistance;
		}
	}
}