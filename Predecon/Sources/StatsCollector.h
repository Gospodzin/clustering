#pragma once
#include <map>

#include "Point.h"
#include "Cluster.h"
#include "Subspace.h"

class StatsCollector
{
public:
	std::map<CId, int> collect(std::vector<Point>& data);
	std::map<CId, int> collect(Clusters clusters);
	std::map<Subspace, std::map<CId, int> > collect(std::map<Subspace, Clusters> clustersBySubspace);
};

