#include "Point.h"
#include <numeric>
#include <algorithm>
#include <functional>


namespace measures{
	typedef  double(*Measure)(const Point&, const Point&);

	double euclideanDistance(const Point& p1, const Point& p2) {
		double res = 0;
		for (int i = 0; i < p1.size(); ++i)
		{
			double diff = p1[i] - p2[i];
			res += diff*diff;
		}
		return std::sqrt(res);
	}

	double manhattanDistance(const Point& p1, const Point& p2) {
		double res = 0;
		for (int i = 0; i < p1.size(); ++i)
			res += std::abs(p1[i] - p2[i]);
		return res;
	}
}