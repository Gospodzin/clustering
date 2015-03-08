#include "Point.h"
#include <sstream>

Point::Point(int dimsCount, int id) : id(id), cid(NONE) {
	reserve(dimsCount);
}

std::string Point::toString() {
	std::stringstream ss;
	for (double& v : *this)
		ss << " " << v;
	return ss.str();
}

