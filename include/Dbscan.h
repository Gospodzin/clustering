#pragma once
#include "logging.h"
#include "datasets.h"
#include <map>
#include <numeric>
#include "Cluster.h"
#include "utils.h"
#include "Algorithm.h"


template <typename T>
class Dbscan : public Algorithm{
public:
	Dbscan(T* dataSet, Params params) : dataSet(dataSet), eps(params.eps), mi(params.mi), attrs(params.attrs) {}

	T* const dataSet;
	const double eps;
	const unsigned mi;
	std::vector<int> attrs;
	std::map < Subspace, Clusters > clustersBySubspace;

	void compute() {
		TS("Performing Dbscan...");
		dbscan();
		TP("Dbscan performed");
	}

	std::map < Subspace, Clusters > getClusters() {
		std::map < Subspace, Clusters > clustersBySubspace;

		Subspace subspace = attrs.empty() ? utils::attrsFromData(dataSet->data) : attrs;

		Clusters clusters = utils::dataToClusters(dataSet->data);

		clustersBySubspace.emplace(subspace, clusters);

		return clustersBySubspace;
	}

	void clean() {
		for (Point& p : *dataSet->data) p.cid = NONE;
	}

private:
	int clusterId = NOISE + 1;
	std::vector<Point*> seeds;

	void dbscan() {
		for (int i = 0; i < dataSet->size(); ++i) {
			Point* point = &(*dataSet)[i];
			if (point->cid == NONE)
				if (expandCluster(point))
					++clusterId;
		}
	}

	bool expandCluster(Point* point) {
		std::vector<Point*> ngb = dataSet->regionQuery(*point, eps, attrs);
		if (ngb.size() < mi) { // not a core point
			point->type = Point::Type::EDGE_OR_NOISE;
			point->cid = NOISE;
			return false; // cluster not found
		}
		else { // core point
			point->type = Point::Type::CORE;
			for (Point* neighbour : ngb) {
				if (neighbour->cid == NONE && neighbour->id != point->id) seeds.push_back(neighbour);
				neighbour->cid = clusterId;
			}

			while (!seeds.empty()) { // expand cluster as far as possible
				Point* seed = seeds.back();
				seeds.pop_back();
				std::vector<Point*> seedNgb = dataSet->regionQuery(*seed, eps, attrs);
				if (seedNgb.size() >= mi) { // core point
					seed->type = Point::Type::CORE;
					for (Point* neighbour : seedNgb)
						if (neighbour->cid == NONE || neighbour->cid == NOISE) {
							if (neighbour->cid == NONE) seeds.push_back(neighbour);
							neighbour->cid = clusterId;
						}
				}
				else {
					seed->type = Point::Type::EDGE_OR_NOISE;
				}
			}

			return true; // cluster found
		}
	}
};
