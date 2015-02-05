#pragma once
#include "Point.h"

struct Cluster {
	Cluster(int cid) : cid(cid){}
	std::vector<Point*> points;
	int cid;
};