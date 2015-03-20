#pragma once
#include "Point.h"

struct Cluster {
	Cluster(CId cid) : cid(cid){}
	std::vector<Point*> points;
	CId cid;
};

typedef std::map<int, Cluster*> Clusters;
