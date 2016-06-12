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
	enum Type {
		UNKNOWN,
		EDGE_OR_NOISE,
		CORE
	};

    Point();
	Point(size_t size);
	Point(int dimsCount, PointId id);
	Point(std::vector<double> data, PointId id);

	PointId id;
	CId cid;
	Type type;

	std::string toString();
};


