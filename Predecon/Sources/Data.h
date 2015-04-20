#pragma once
#include "Point.h"

struct Data : std::vector<Point> {
	Data() {}
	Data(size_t size, Point point) : std::vector<Point>(size, point) {}


	std::vector<std::string> headers;

	size_t dimensions() { return front().size(); }
};