#pragma once
#include <map>
#include <set>

#include "logging.h"
#include "Dbscan.h"
#include "ODC.h"
#include "Subspace.h"

template<typename T>
class Subclu {
public:
	Subclu(std::vector<Point>* data, double eps, int mi, bool odc = false) : data(*data), eps(eps), mi(mi), odc(odc) {}

	std::vector<Point>& data;
	const double eps;
	const int mi;
	std::map < Subspace, Clusters > clustersBySubspace;


	void compute() {
		clustersBySubspace = subclu();
	}

	std::map < Subspace, Clusters > getClusters() {
		return clustersBySubspace;
	}

	void clean() {
        for(Point& p : data) p.cid = NONE;
	}


private:
	std::vector<int> idCache;
	bool odc;
    int subspaceCounter = 0;

	std::map < Subspace, Clusters > subclu() {
		// STEP 1 Generate all 1-D clusters
		std::map < Subspace, Clusters > clustersBySubspace;
		Subspaces withClusters;
		for(int a = 0; a < data.front().size(); ++a) {
			Subspace subspace(1, a);
			Clusters clusters;

			if(odc) {
				ODC odc(&data, eps, mi, a);
				odc.compute();
				clusters = odc.getClusters().begin()->second;
				odc.clean();
			}
			else {
				T dataSet(&data, measures::MeasureId::Euclidean, referenceSelectors::max, subspace);
				Dbscan<T> dbscan(&dataSet, eps, mi, subspace);
				dbscan.compute();
				clusters = dbscan.getClusters().begin()->second;
				dbscan.clean();
			}
            ++subspaceCounter;

			if(!clusters.empty()) {
				clustersBySubspace.emplace(subspace, clusters);
				withClusters.push_back(subspace);
			}
		}
		// STEP 2 Generate (k+1)-D clusters from k-D clusters
		std::map < Subspace, Clusters > totalClustersBySubspace;
		totalClustersBySubspace.insert(clustersBySubspace.begin(), clustersBySubspace.end());
		while(!withClusters.empty()) {
			Subspaces newWithClusters;
			std::map < Subspace, Clusters > newClustersBySubspace;

			// STEP 2.1 Generate (k+1)-D candidate subspaces
			Subspaces candidates = genCandidates(withClusters);

			// STEP 2.2 Test candidates and generate (k+1)-D clusters
			for(Subspace cand : candidates) {
				Subspace bestSub = minimalSubspace(cand, clustersBySubspace);
				Clusters candClusters;
				for(auto cluster : clustersBySubspace[bestSub]) {
					std::vector<Point> clusterData = getData(cluster.second);
					T dataSet(&clusterData, measures::MeasureId::Euclidean, referenceSelectors::max, cand);
					Dbscan<T> dbscan(&dataSet, eps, mi, cand);
                    ++subspaceCounter;
					dbscan.compute();
					Clusters clusters = convert(dbscan.getClusters().begin()->second);
					dbscan.clean();
					candClusters.insert(clusters.begin(), clusters.end());
				}
				if(!candClusters.empty()) {
					newWithClusters.push_back(cand);
					newClustersBySubspace.emplace(cand, candClusters);
				}
			}

			if(!newClustersBySubspace.empty()) totalClustersBySubspace.insert(newClustersBySubspace.begin(), newClustersBySubspace.end());

			clustersBySubspace = newClustersBySubspace;
			withClusters = newWithClusters;
		}

		return totalClustersBySubspace;
	}

	Clusters convert(Clusters clusters) {
		Clusters orgC;
		for(auto cluster : clusters) {
			Cluster* n = new Cluster(cluster.second->cid);
			n->points.reserve(cluster.second->points.size());
			for(Point* point : cluster.second->points) {
				Point* org = &(data[idCache[point->id]]);
				n->points.push_back(org);
			}
			orgC.emplace(n->cid, n);
			delete cluster.second;
		}
		return orgC;
	}

	std::vector<Point> getData(Cluster* cluster) {
		std::vector<Point> data;
		for(Point* p : cluster->points)
			data.push_back(*p);
		idCache.clear();
		for(size_t i = 0; i < data.size(); ++i) {
			idCache.push_back(data[i].id);
			data[i].id = i;
		}
		return data;
	}

	Subspace minimalSubspace(Subspace& cand, std::map < Subspace, Clusters >& clustersByAttrs) {
		Subspace minimal;
		int minNo = 1 << 30;
		for(size_t i = 0; i < cand.size(); ++i) {
			Subspace sub = cand;
			sub.erase(sub.begin() + i);
			Clusters& clusters = clustersByAttrs[sub];
			int curNo = countObjects(clusters);
			if(curNo < minNo) {
				minimal = sub;
				minNo = curNo;
			}
		}
		return minimal;
	}

	int countObjects(Clusters& clusters) {
		int count = 0;
		for(auto c : clusters)
			count += c.second->points.size();
		return count;
	}

	Subspaces genCandidates(Subspaces withClusters) {
		// STEP 2.1.1 Generate (k+1)-D candidate subspaces
		Subspaces candidates;
		for(Subspace s1 : withClusters) {
			for(Subspace s2 : withClusters) {
				if(arePairable(s1, s2)) {
					Subspace candidate = s1;
					candidate.push_back(s2.back());
					candidates.push_back(candidate);
				}
			}
		}

		// STEP 2.1.2 Prune irrelevant candidates subspaces
		std::set<Subspace> withClustersSet(withClusters.begin(), withClusters.end());
		for(size_t i = 0; i < candidates.size(); ++i)
			if(pruneCand(candidates[i], withClustersSet))
				candidates.erase(candidates.begin() + i--);

		return candidates;
	}

	bool pruneCand(Subspace& cand, std::set<Subspace>& withClusters) {
		for(size_t i = 0; i < cand.size(); ++i) {
			Subspace sub = cand;
			sub.erase(sub.begin() + i);
			if(withClusters.find(sub) == withClusters.end()) return true;
		}
		return false;
	}

	bool arePairable(Subspace& s1, Subspace& s2) {
		for(size_t i = 0; i < s1.size() - 1; ++i)
			if(s1[i] != s2[i]) return false;

		if(s1.back() >= s2.back()) return false;

		return true;
	}
};
