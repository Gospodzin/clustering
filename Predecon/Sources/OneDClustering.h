#pragma once

#include "Point.h"
#include "Cluster.h"
#include <algorithm>
#include <queue>

// deprecated
class OneDClustering {
public:
	OneDClustering(std::vector<Point>* data, double eps, int mi, int attr) : data(data), eps(eps), mi(mi), attr(attr) {
		sortedData.reserve(data->size());
		for (Point& p : *data) sortedData.emplace_back(&p);
		std::sort(sortedData.begin(), sortedData.end(), [&](Point const* const& p1, Point const* const& p2) -> bool{ return p1->at(attr) < p2->at(attr); });
	}

	void compute() {
		std::deque<Point*> fifol;
		std::deque<Point*> fifor;

		Point* cur;
		bool clusterFound = false;

		int cid = NOISE + 1;

		for (int i = 0; i < sortedData.size(); ++i) {
			Point* p = sortedData[i];
			if (fifol.empty() || dist(fifol.front(), p) <= eps) {
				fifol.push_back(p);
				cur = p;
			}
			else if (dist(cur, p) <= eps) {
				fifor.push_back(p);
			}
			else {
				fifor.push_back(p);
				while (!fifor.empty() && dist(cur, p) > eps) {
					cur = fifor.front();
					fifor.pop_front();
					fifol.push_back(cur);
				}
				while (dist(fifol.front(), cur) > eps) fifol.pop_front();
				if (cur->cid == NONE && clusterFound) ++cid, clusterFound = false;
			}

			if (fifol.size() + fifor.size() >= mi) {
				if (fifor.size() > 0 && fifor.back()->cid != cid) {
					clusterFound = true;
					for (auto it = fifor.rbegin(); it != fifor.rend() && (*it)->cid != cid; ++it)
						(*it)->cid = cid;
				}

				if (fifol.size() > 0 && fifol.back()->cid != cid) {
					clusterFound = true;
					for (auto it = fifol.rbegin(); it != fifol.rend() && (*it)->cid != cid; ++it)
						(*it)->cid = cid;
				}
			}
		}

		for (Point* p : sortedData)
			if (p->cid == NONE)
				p->cid = NOISE;
	}

	std::vector<Cluster*> getClusters() {
		LOG("Performing OneDClustering...");
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

	double dist( Point const* const& p1,  Point const* const & p2) const {
		return std::abs(p1->at(attr) - p2->at(attr));
	}
};