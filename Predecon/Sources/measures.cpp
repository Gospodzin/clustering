#include "measures.h"

namespace measures{
	double euclideanDistance(const Point& p1, const Point& p2) {
        double res = 0;
        for (int i = 0; i < (int)p1.size(); ++i) {
            double diff = p1[i] - p2[i];
            res += diff*diff;
        }
        return sqrt(res);
	}

	double cosDistance(const Point& p1, const Point& p2) {
		double num = 0, suma = 0, sumb = 0;
		for (int i = 0; i < (int)p1.size(); ++i) 
            num += p1[i] * p2[i], suma += p1[i]*p1[i], sumb += p2[i]*p2[i];
        double cos = 1.0-num/(sqrt(suma)*sqrt(sumb));
        return cos;
	}

	double euclideanDistance(const Point& p1, const Point& p2, const std::vector<double>& prefVector) {
		double res = 0;
		for (int i = 0; i < (int)p1.size(); ++i) {
			double diff = p1[i] - p2[i];
			res += prefVector[i] * diff * diff;
		}
		return sqrt(res);
	}

	double euclideanDistance(const Point& p1, const Point& p2, const std::vector<int>& attrs) {
		double res = 0;
		for (int i = 0; i < (int)attrs.size(); ++i) {
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

	double manhattanDistance(const Point& p1, const Point& p2, const std::vector<double>& prefVector) {
		double res = 0;
		for (int i = 0; i < (int)p1.size(); ++i)
			res += prefVector[i] * std::abs(p1[i] - p2[i]);
		return res;
	}

	double manhattanDistance(const Point& p1, const Point& p2, const std::vector<int>& attrs) {
		double res = 0;
		for (int i = 0; i < (int)attrs.size(); ++i)
			res += std::abs(p1[attrs[i]] - p2[attrs[i]]);
		return res;
	}

	Measure getMeasure(MeasureId measureId) {
		switch (measureId) {
		case Euclidean:
			return euclideanDistance;
		case Manhattan:
			return manhattanDistance;
		case CosDist:
			return cosDistance;
		default:
			return NULL;
		}
	}

	PrefMeasure getPrefMeasure(MeasureId measureId) {
		switch (measureId) {
		case Euclidean:
			return euclideanDistance;
		case Manhattan:
			return manhattanDistance;
		default:
			return NULL;
		}
	}

	AttrsMeasure getAttrsMeasure(MeasureId measureId) {
		switch (measureId) {
		case Euclidean:
			return euclideanDistance;
		case Manhattan:
			return manhattanDistance;
		default:
			return NULL;
		}
	}

}
