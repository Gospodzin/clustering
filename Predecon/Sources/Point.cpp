#include "Point.h"
#include <sstream>

Point::Point() {}

Point::Point(int dimsCount, PointId id) : id(id), cid(NONE) { reserve(dimsCount); }

Point::Point(std::vector<double> data, PointId id) : id(id), cid(NONE), vector<double>(data) {}

std::string Point::toString() {
	std::stringstream ss;
	bool first = true;
	for(double& v : *this) {
		if(!first) ss << " ";
		else first = false;
		ss << v;
	}
	return ss.str();
}

