#pragma once
#include <vector>


const int NONE = -1;
const int NOISE = 0;

struct Point : std::vector<double>
{
public:
	Point(int dimsCount, int id);

	int id;
	int cid;

	std::string toString();
};

