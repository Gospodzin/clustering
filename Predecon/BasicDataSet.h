#pragma once
#include "DataSet.h"

struct BasicDataSet : DataSet
{
	std::vector<Point> data;
	
	BasicDataSet(int n) { 
		data.reserve(n); 
	}

	inline std::vector<Point>::iterator begin() {
		return data.begin();
	}

	inline std::vector<Point>::iterator end() {
		return data.end();
	}

	inline Point& operator[](const int& n) {
		return data[n];
	}

	inline std::vector<Point*> regionQuery(const int& pointOrderId, const double& eps, measures::Measure measure) {
		std::vector<Point*> neighbours;
		Point& target = data[pointOrderId];
		for (Point& p : data)
			if (measure(target, p) <= eps)
				neighbours.push_back(&p);
		return neighbours;
	}
};

