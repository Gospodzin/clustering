#pragma once
#include "logging.h"
#include "datasets.h"
#include <map>
#include <numeric>
#include "Cluster.h"
#include "utils.h"


template <typename T>
class Dbscan {
public:
	Dbscan(T* dataSet, double eps, unsigned mi, std::vector<int> attrs = {}) : dataSet(dataSet), eps(eps), mi(mi), attrs(attrs) {}

	T* const dataSet;
	const double eps;
	const unsigned mi;
	std::vector<int> attrs;
	std::map < Subspace, Clusters > clustersBySubspace;

	void compute() {
		dbscan();
	}

	std::map < Subspace, Clusters > getClusters() {
		std::map < Subspace, Clusters > clustersBySubspace;

		Subspace subspace = attrs.empty() ? utils::attrsFromData(dataSet->data) : attrs;

		Clusters clusters = utils::dataToClusters(dataSet->data);

		clustersBySubspace.emplace(subspace, clusters);

		return clustersBySubspace;
	}

	void clean() {
		for(Point& p : *dataSet->data) p.cid = NONE;
	}

private:
	int clusterId = NOISE + 1;
	std::vector<Point*> seeds;

	void dbscan() {
		LOG("Performing Dbscan...")
			TS()
			for(int i = 0; i < dataSet->size(); ++i) {
				Point* point = &(*dataSet)[i];
				if(point->cid == NONE)
					if(expandCluster(point))
						++clusterId;
			}
		TP()
	}

	bool expandCluster(Point* point) {
		std::vector<Point*> ngb = dataSet->regionQuery(*point, eps, attrs);
		if(ngb.size() < mi) { // not a core point
			point->cid = NOISE;
			return false; // cluster not found
		}
		else { // core point
			point->cid = clusterId;
			for(Point* neighbour : ngb) {
				neighbour->cid = clusterId;
				seeds.push_back(neighbour);
			}

			while(!seeds.empty()) { // expand cluster as far as possible
				Point* seed = seeds.back();
				seeds.pop_back();
				std::vector<Point*> seedNgb = dataSet->regionQuery(*seed, eps, attrs);
				if(seedNgb.size() >= mi) // core point
					for(Point* neighbour : seedNgb)
						if(neighbour->cid == NONE || neighbour->cid == NOISE) {
							neighbour->cid = clusterId;
							seeds.push_back(neighbour);
						}
			}

			return true; // cluster found
		}
	}
};
