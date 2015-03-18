#pragma once
#include <map>
#include <numeric>
#include "Cluster.h"
#include "Subspace.h"

namespace utils {
	static Clusters dataToClusters(std::vector<Point>* data) {
		std::map<int, Cluster*> clustersById;
		Clusters clusters;
		for(Point& p : *data) {
			if(p.cid != NOISE) {
				if(clustersById.find(p.cid) != clustersById.end()) {
					clustersById[p.cid]->points.push_back(&p);
				}
				else {
					clusters.push_back(new Cluster(p.cid));
					clustersById.emplace(p.cid, clusters.back());
					clustersById[p.cid]->points.push_back(&p);
				}
			}
		}

		for(Point& p : *data)  p.cid = NONE;

		return clusters;
	}

	static Subspace attrsFromData(std::vector<Point>* data) {
		Subspace attrs(data->front().size());
		std::iota(attrs.begin(), attrs.end(), 0);

		return attrs;
	}
}