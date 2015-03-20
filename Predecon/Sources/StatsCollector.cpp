#include "StatsCollector.h"
#include <fstream>
#include "logging.h"
#include <sstream>
#include <map>

std::map<CId, int> StatsCollector::collect(std::vector<Point>& data) {
	LOG("Collecting statistics...");
	TS();
	std::map<CId, int> stats;
	for (Point& p : data)
        if(p.cid != NOISE) {
            if (stats.find(p.cid) == stats.end())
                stats.emplace(p.cid, 1);
            else
                ++stats[p.cid];
        }
	TP();
	return stats;
}

std::map<CId, int> StatsCollector::collect(Clusters clusters) {
	std::map<CId, int> stats;
	for (auto cluster : clusters)
		stats.emplace(cluster.second->cid, cluster.second->points.size());

	return stats;
}

std::map<Subspace, std::map<CId, int> > StatsCollector::collect(std::map<Subspace, Clusters> clustersBySubspace) {
	std::map<Subspace, std::map<CId, int> > statsBySubspace;
    for (auto clusters : clustersBySubspace)
        statsBySubspace.emplace(clusters.first, collect(clusters.second));
	
	return statsBySubspace;
}
