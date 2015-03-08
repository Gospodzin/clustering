#pragma once
#include "Point.h"

namespace referenceSelectors {
	typedef  Point(*ReferenceSelector)(std::vector<Point>& data);

	Point max(std::vector<Point>& data);
	Point min(std::vector<Point>& data);
}