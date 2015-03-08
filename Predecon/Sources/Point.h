#pragma once
#include <vector>

typedef int CId;

const CId NONE = -1;
const CId NOISE = 0;

struct Point : std::vector<double>
{
public:
	Point(int dimsCount, int id);

	int id;
	CId cid;

	std::string toString();
};

