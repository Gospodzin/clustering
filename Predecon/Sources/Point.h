#pragma once
#include <vector>

struct Point;

typedef int CId;
typedef int PointId;

const CId NONE = -1;
const CId NOISE = 0;

struct Point : std::vector<double>
{
public:
    Point();
	Point(int dimsCount, PointId id);
	Point(std::vector<double> data, PointId id);

	PointId id;
	CId cid;

	std::string toString();
};


