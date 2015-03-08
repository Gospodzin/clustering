#pragma once
#include "logging.h"
#include "datasets.h"
#include <map>
#include "Cluster.h"


template <typename T>
class Dbscan
{
public:
	Dbscan(T* data, double eps, unsigned mi, std::vector<int> attrs) : data(data), eps(eps), mi(mi), attrs(attrs) {}
        Dbscan(T* data, double eps, unsigned mi) : Dbscan(data, eps, mi, std::vector<int>(data->size())) {
		std::iota(attrs.begin(), attrs.end(), 0);
	}


	T* const data;
	const double eps;
	const unsigned mi;
	std::vector<int> attrs;

	void compute() {
		dbscan();
	}

	std::vector<Cluster*> getClusters() {
		dbscan();

		std::map<int, Cluster*> clustersById;
		std::vector<Cluster*> clusters;
		for(Point& p : *(data->data)) {
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

		for (Point& p : *(data->data))  p.cid = NONE;

		return clusters;
	}

private:
	int clusterId = NOISE + 1;
	std::vector<Point*> seeds;

	void dbscan() {
		for (int i = 0; i < data->size(); ++i) {
			Point* point = &(*data)[i];
			if (point->cid == NONE) 
				if (expandCluster(point)) 
					++clusterId;
		}
	}

	bool expandCluster(Point* point) {
		std::vector<Point*> ngb = data->regionQuery(*point, eps, attrs);
		if (ngb.size() < mi) { // not a core point
			point->cid = NOISE;
			return false; // cluster not found
		}
		else { // core point
			point->cid = clusterId;
			for (Point* neighbour : ngb) {
				neighbour->cid = clusterId;
				seeds.push_back(neighbour);
			}

			while (!seeds.empty()) { // expand cluster as far as possible
				Point* seed = seeds.back();
				seeds.pop_back();
				std::vector<Point*> seedNgb = data->regionQuery(*seed, eps, attrs);
				if(seedNgb.size() >= mi) // core point
					for(Point* neighbour : seedNgb)
						if (neighbour->cid == NONE || neighbour->cid == NOISE) {
							neighbour->cid = clusterId;	
							seeds.push_back(neighbour);
						}
			}
			
			return true; // cluster found
		}
	}
};
