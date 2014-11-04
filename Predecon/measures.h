#pragma once
#include "Point.h"

namespace measures{
	typedef  double(*Measure)(const Point&, const Point&);
	typedef  double(*PrefMeasure)(const Point&, const Point&, std::vector<double>& prefVector);


	 double euclideanDistanceSquared(const Point& p1, const Point& p2) {
		double res = 0;
		for (int i = 0; i < (int)(int)p1.size(); ++i) {
			double diff = p1[i] - p2[i];
			res += diff*diff;
		}
		return res;
	}

	double euclideanDistanceSquared(const Point& p1, const Point& p2, std::vector<double>& prefVector) {
		double res = 0;
		for (int i = 0; i < (int)p1.size(); ++i) {
			double diff = p1[i] - p2[i];
			res += prefVector[i] * diff * diff;
		}
		return res;
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
}