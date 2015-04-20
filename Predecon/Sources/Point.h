#pragma once
#include <vector>

struct Point;

typedef int CId;

const CId NONE = -1;
const CId NOISE = 0;

struct Point : std::vector<double>
{
public:
    Point();
	Point(int dimsCount, int id);
	Point(std::vector<double> data, int id);

	int id;
	CId cid;

	std::string toString();
};


