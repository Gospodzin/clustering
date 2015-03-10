#pragma once

#include <algorithm>
#include <queue>
#include "Point.h"
#include "Cluster.h"
#include "logging.h"

class ODC {
public:
	ODC(std::vector<Point>* data, int attr, double eps, int mi) : data(data), attr(attr), eps(eps), mi(mi) {
		sortedData.reserve(data->size());
		for (Point& p : *data) sortedData.emplace_back(&p);
		std::sort(sortedData.begin(), sortedData.end(), [&](Point const* const& p1, Point const* const& p2) -> bool{ return p1->at(attr) < p2->at(attr); });
	}

	void compute() {
		int cid = NOISE + 1;
		int l = 0, r = 0;
		bool clusterFound = false;
		for (int i = 0; i < sortedData.size(); ++i) {
			Point* cur = sortedData[i];
			r = r < i ? i : r;
			l = l > i ? i : l;
			while (dist(cur, sortedData[l]) > eps)
				++l;
			while (r < sortedData.size() && dist(cur, sortedData[r]) <= eps)
				++r;
			if (cur->cid == NONE && clusterFound) ++cid, clusterFound = false;
			if (r-- - l >= mi)
				for (int j = r; j >= l && sortedData[j]->cid != cid; --j)
					sortedData[j]->cid = cid, clusterFound = true;
		}

		for (Point* p : sortedData)
			if (p->cid == NONE)
				p->cid = NOISE;
	}

	std::vector<Cluster*> getClusters() {
		LOG("Performing ODC...");
		TS();
		compute();
		TP();

		LOG("Collecting clusters and cleaning data...");
		TS();
		std::map<int, Cluster*> clustersById;
		std::vector<Cluster*> clusters;
		for (Point& p : *data) {
			if (p.cid != NOISE) {
				if (clustersById.find(p.cid) != clustersById.end()) {
					clustersById[p.cid]->points.push_back(&p);
				}
				else {
					clusters.push_back(new Cluster(p.cid));
					clustersById.emplace(p.cid, clusters.back());
					clustersById[p.cid]->points.push_back(&p);
				}
			}
		}

		for (Point& p : *data)  p.cid = NONE;
		TP();
		return clusters;
	}
private:
	int attr;
	double eps;
	int mi;
	std::vector<Point*> sortedData;
	std::vector<Point>* data;

	double dist(Point const* const& p1, Point const* const & p2) const {
		return std::abs(p1->at(attr) - p2->at(attr));
	}
};