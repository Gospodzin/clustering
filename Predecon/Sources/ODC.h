#pragma once

#include <algorithm>
#include <queue>
#include "Point.h"
#include "Cluster.h"
#include "logging.h"

class ODC {
public:
	ODC(std::vector<Point>* data, double eps, int mi, int attr) : data(data), eps(eps), mi(mi), attr(attr) {
		sortedData.reserve(data->size());
		for(Point& p : *data) sortedData.emplace_back(&p);
		std::sort(sortedData.begin(), sortedData.end(), [&](Point const* const& p1, Point const* const& p2) -> bool { return p1->at(attr) < p2->at(attr); });
	}

	void compute() {
		LOG("Performing Dbscan...");
		TS();
		odc();
		TP();
	}

	std::map < Subspace, Clusters > getClusters() {
		std::map < Subspace, Clusters > clustersBySubspace;

		Subspace subspace = {attr};

		Clusters clusters = utils::dataToClusters(data);

		clustersBySubspace.emplace(subspace, clusters);

		return clustersBySubspace;
	}

	void clean() {
		for(Point& p : *data) p.cid = NONE;
	}

private:
	int attr;
	double eps;
	int mi;
	std::vector<Point*> sortedData;
	std::vector<Point>* data;

	void odc() {
		int cid = NOISE + 1;
		int l = 0, r = 0;
		bool clusterFound = false;
		for(int i = 0; i < sortedData.size(); ++i) {
			Point* cur = sortedData[i];
			r = r < i ? i : r;
			l = l > i ? i : l;
			while(dist(cur, sortedData[l]) > eps) ++l;
			while(r < sortedData.size() && dist(cur, sortedData[r]) <= eps) ++r;
			if(cur->cid == NONE && clusterFound) ++cid, clusterFound = false;
			if(r-- - l >= mi)
				for(int j = r; j >= l && sortedData[j]->cid != cid; --j)
					sortedData[j]->cid = cid, clusterFound = true;
		}

		for(Point* p : sortedData)
			if(p->cid == NONE)
				p->cid = NOISE;
	}

	double dist(Point const* const& p1, Point const* const & p2) const {
		return std::abs(p1->at(attr) - p2->at(attr));
	}
};
