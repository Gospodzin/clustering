#pragma once
#include <map>
#include <numeric>
#include "Cluster.h"
#include "Subspace.h"

namespace utils {
	static Clusters dataToClusters(std::vector<Point>* data) {
		Clusters clusters;
		for(Point& p : *data) {
			if(p.cid != NOISE) {
				if(clusters.find(p.cid) != clusters.end()) {
					clusters[p.cid]->points.push_back(&p);
				}
				else {
					clusters.emplace(p.cid, new Cluster(p.cid));
					clusters[p.cid]->points.push_back(&p);
				}
			}
		}

		return clusters;
	}

	static int countNoise(Clusters& clusters, Data* data) {
		int clusteredCount = 0;
		for(auto cluster : clusters) clusteredCount += cluster.second->points.size();

		int noiseCount = data->size() - clusteredCount;

		return noiseCount;
	}

	static Subspace attrsFromData(std::vector<Point>* data) {
		Subspace attrs(data->front().size());
		std::iota(attrs.begin(), attrs.end(), 0);

		return attrs;
	}
}