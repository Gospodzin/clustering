#pragma once
#include "logging.h"
#include "Dbscan.h"
#include <map>
#include <set>

typedef std::vector <int> Subspace;
typedef std::vector <Subspace> Subspaces;
typedef std::vector<Cluster*> Clusters;

//template <typename T>
class Subclu
{
public:
	Subclu(BasicDataSet* data, double eps, int mi) : data(data), eps(eps), mi(mi) {}

	BasicDataSet* const data;
	const double eps;
	const int mi;

	void run() {
		subclu();
	}
private:
	std::vector < std::map < Subspace, Clusters > > subclu() {
		// STEP 1 Generate all 1-D clusters
		std::map < Subspace, Clusters > clustersByAttrs;
		Subspaces withClusters;
		for (int a = 0; a < data->dimensions(); ++a) {
			Subspace attrs(1, a);
			auto clusters = Dbscan(data, eps, mi, attrs).getClusters();
			if (!clusters.empty()) {
				clustersByAttrs.emplace(attrs, clusters);
				withClusters.push_back(attrs);
			}
		}
		// STEP 2 Generate (k+1)-D clusters from k-D clusters
		std::vector < std::map < Subspace, Clusters > > clustersByDim;
		clustersByDim.push_back(clustersByAttrs);
		while (!withClusters.empty()) {
			Subspaces newWithClusters;
			std::map < Subspace, Clusters > newClustersByAttrs;

			// STEP 2.1 Generate (k+1)-D camdodate sinspaces
			Subspaces candidates = genCandidates(withClusters);
			
			// STEP 2.2 Test candidates and generate (k+1)-D clusters
			for (Subspace cand : candidates) {
				Subspace bestSub = minimalSubspace(cand, clustersByAttrs);
				Clusters candClusters;
				for (Cluster* cluster : clustersByAttrs[bestSub]) {
					std::vector<Point> clusterData = getData(cluster);
					BasicDataSet dataSet(&clusterData, measures::euclideanDistance);
					Clusters clusters = convert(Dbscan(&dataSet, eps, mi, cand).getClusters());
					candClusters.reserve(candClusters.size() + clusters.size());
					candClusters.insert(candClusters.end(), clusters.begin(), clusters.end());
				}
				if (!candClusters.empty()) {
					newWithClusters.push_back(cand);
					newClustersByAttrs.emplace(cand, candClusters);
				}
			}

			if (!withClusters.empty())clustersByDim.push_back(newClustersByAttrs);
			clustersByAttrs = newClustersByAttrs;
			withClusters = newWithClusters;
		}

		return clustersByDim;
	}

	Clusters convert(Clusters clusters) {
		Clusters orgC;
		for (Cluster* cluster : clusters) {
			Cluster* n = new Cluster(cluster->cid);
			n->points.reserve(cluster->points.size());
			for (Point* point : cluster->points) {
				Point* org = &((*(data->data))[point->id]);
				n->points.push_back(org);
			}
			orgC.push_back(n); 
			delete cluster;
		}
		return orgC;
	}

	std::vector<Point> getData(Cluster* cluster) {
		std::vector<Point> data;
		for (Point* p : cluster->points)
			data.push_back(*p);
		return data;
	}

	Subspace minimalSubspace(Subspace& cand, std::map < Subspace, std::vector<Cluster*> >& clustersByAttrs) {
		Subspace minimal;
		int minNo = 1 << 30;
		for (int i = 0; i < cand.size(); ++i) {
			Subspace sub = cand;
			sub.erase(sub.begin() + i);
			std::vector<Cluster*>& clusters = clustersByAttrs[sub];
			int curNo = countObjects(clusters);
			if (curNo < minNo) {
				minimal = sub;
				minNo = curNo;
			}
		}
		return minimal;
	}

	int countObjects(std::vector<Cluster*>& clusters) {
		int count = 0;
		for (Cluster* c : clusters)
			count += c->points.size();
		return count;
	}

	Subspaces genCandidates(Subspaces withClusters) {
		// STEP 2.1.1 Generate (k+1)-D candidate subspaces
		Subspaces candidates;
		for (Subspace s1 : withClusters) {
			for (Subspace s2 : withClusters) {
				if (arePairable(s1,s2)) {
					Subspace candidate = s1;
					candidate.push_back(s2.back());
					candidates.push_back(candidate);
				}
			}
		}

		// STEP 2.1.2 Prune irrelevant candidates subspaces
		std::set<Subspace> withClustersSet(withClusters.begin(), withClusters.end());
		for (int i = 0; i < candidates.size(); ++i)
			if (pruneCand(candidates[i], withClustersSet))
				candidates.erase(candidates.begin() + i--);

		return candidates;
	}

	bool pruneCand(Subspace& cand, std::set<Subspace>& withClusters) {
		for (int i = 0; i < cand.size(); ++i) {
			Subspace sub = cand;
			sub.erase(sub.begin() + i);
			if (withClusters.find(sub) == withClusters.end()) return true;
		}
		return false;
	}

	bool arePairable(Subspace& s1, Subspace& s2) {
		for (int i = 0; i < s1.size() - 1; ++i)
			if (s1[i] != s2[i]) return false;

		if (s1.back() >= s2.back()) return false;

		return true;
	}
};