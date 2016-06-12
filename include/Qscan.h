#pragma once
#include "Dbscan.h"
#include "PLDataSet.h"
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <queue>

class Qscan {
public:
    Qscan(std::vector<Point>* data, double eps, int mi, int n, int divs) : data(*data), eps(eps), mi(mi), n(n), divs(divs), ref(referenceSelectors::max(*data)) {}

	std::vector<Point>& data;
	const double eps;
	const int mi;
	const int n;
	const int divs;
	std::map < Subspace, Clusters > clustersBySubspace;
    Point ref;


	void compute() {
        TS("Performing Qscan...");
        qscan();
        TP("Qscan performed");
	}

    std::map < Subspace, Clusters > getClusters() {
        std::map < Subspace, Clusters > clustersBySubspace;

        Subspace subspace = utils::attrsFromData(&data);

        Clusters clusters = utils::dataToClusters(&data);

        clustersBySubspace.emplace(subspace, clusters);

        return clustersBySubspace;
    }

	void clean() {
        for(Point& p : data) p.cid = NONE;
	}


private:
    void qscan() {
		std::map < Subspace, Clusters > clustersBySubspace;

		int divDim = selectDivDim(data);
		double median = getMedian(data, divDim);
		LOG("DivDim: " + std::to_string(divDim));
		LOG("Median: " + std::to_string(median));

        std::vector<Point> L;
		L.reserve(data.size());
        std::vector<Point> R;
        R.reserve(data.size());

		for (int i = 0; i < data.size(); ++i) { 
			if (data[i][divDim] <= median + 1.5*eps) L.emplace_back(data[i]);
			if (data[i][divDim] >= median - 1.5*eps) R.emplace_back(data[i]);
		}

        performDbscan(L);
        performDbscan(R);

        merge(L, R, divDim, median);
	}

	double getMedian(std::vector<Point>& data, int dim) {
		std::vector<Point*> dataCpy;
		dataCpy.reserve(data.size());
		for (Point& p : data) dataCpy.push_back(&p);

		std::nth_element(dataCpy.begin(), dataCpy.begin() + (dataCpy.size() - 1) / 2, dataCpy.end(), [&](const Point* p1, const Point* p2) -> bool {return p1->at(dim) < p2->at(dim); });
		double median = dataCpy[dataCpy.size() / 2]->at(dim);

		return median;
	}

	int selectDivDim(std::vector<Point>& data) {
		std::vector<double> deviations = utils::calcMeanDeviations(data);

        std::vector<int> dims(data.front().size());
		for (int i = 0; i < data.front().size(); ++i) 
			dims[i] = i;

		std::nth_element(dims.begin(), dims.begin() , dims.end(), [&](const int dim1, const int dim2) -> bool {return deviations[dim1] > deviations[dim2]; });
		int dim = dims[0];

		return dim;
	}

	/* Not used alternative */
	int selectDivDim_2(std::vector<Point>& data) {
		std::vector<double> medians = utils::calcMedians(data);

		std::vector<int> counters(data.front().size());
		for (Point& p : data){
			for (int i = 0; i < medians.size(); ++i)
				if (std::abs(p[i] - medians[i]) <= eps)
					++counters[i];
		}

		int dim = std::distance(counters.begin(), std::min_element(counters.begin(), counters.end()));

		return dim;
	}

	enum Part { L, R };
	struct Node {

		bool discovered = false;
		int cid;
		Part part;
		std::set<Node*> adjacent;

		Node(Part part, int cid) : part(part), cid(cid) {}
	};

	std::vector<Node*> bfs(Node* startNode) {
		std::vector<Node*> component;

		std::queue<Node*> fifo;
		startNode->discovered = true;
		fifo.push(startNode);
		while (!fifo.empty()) {
			Node* node = fifo.front();
			fifo.pop();
			component.emplace_back(node);
			for (Node* adj : node->adjacent)
				if (!adj->discovered) {
					adj->discovered = true;
					fifo.push(adj);
				}
		}

		return component;
	}

	std::vector<std::vector<Node*>> getComponents(std::unordered_map<int, std::shared_ptr<Node>>& lCids) {
		std::vector<std::vector<Node*>> components;
		for (auto& lCidNode : lCids)
			if (!lCidNode.second->discovered)
				components.emplace_back(bfs(lCidNode.second.get()));

		return components;
	}

	std::unordered_map<Part, std::unordered_map<int, int>> getClusterMappings(std::vector<Point>& LM, std::vector<Point>& RM, int cidOffset) {
		std::unordered_map<int, std::shared_ptr<Node>> lCids;
		std::unordered_map<int, std::shared_ptr<Node>> rCids;
		for (int i = 0; i < LM.size(); i++) {
			int lCid = LM[i].cid, rCid = RM[i].cid;
			if (lCid != NOISE && rCid != NOISE && LM[i].type == Point::Type::CORE && RM[i].type == Point::Type::CORE) { // ewentualnie OR, ale chyba jest ok tak jak jest
				if (lCids.find(lCid) == lCids.end()) lCids.emplace(lCid, std::shared_ptr<Node>(new Node(Part::L, lCid)));
				if (rCids.find(rCid) == rCids.end()) rCids.emplace(rCid, std::shared_ptr<Node>(new Node(Part::R, rCid)));

				lCids[lCid]->adjacent.emplace(rCids[rCid].get());
				rCids[rCid]->adjacent.emplace(lCids[lCid].get());
			}
		}

		std::unordered_map<Part, std::unordered_map<int, int>> mappings;
		mappings.emplace(Part::L, std::unordered_map<int, int>());
		mappings.emplace(Part::R, std::unordered_map<int, int>());

		auto components = getComponents(lCids);
		for (int i = 0; i < components.size(); ++i) {
			int cid = cidOffset + i;
			auto& component = components[i];
			for (Node* node : component)
				mappings[node->part].emplace(node->cid, cid);
		}

		return mappings;
	}

	int maxCid(std::vector<Point>& data) {
		return std::max_element(data.begin(), data.end(), [](const Point& p1, const Point& p2) -> bool {return p1.cid < p2.cid; })->cid;
	}

	void merge(std::vector<Point>& L, std::vector<Point>& R, int divDim, double bound) {
        TS("Merging...");
		
		// shift right clusters
		int lMaxCid = maxCid(L);
		for (Point& p : R) p.cid = p.cid == NOISE ? p.cid : p.cid + lMaxCid;

		// create merging fields
		std::vector<Point> LM, RM;
		for (Point& p : L) if (p[divDim] >= bound - 1.5*eps && p[divDim] <= bound + 1.5*eps) LM.emplace_back(p);
		for (Point& p : R) if (p[divDim] >= bound - 1.5*eps && p[divDim] <= bound + 1.5*eps) RM.emplace_back(p);
		std::sort(LM.begin(), LM.end(), [](const Point& p1, const Point& p2) -> bool {return p1.id < p2.id; });
		std::sort(RM.begin(), RM.end(), [](const Point& p1, const Point& p2) -> bool {return p1.id < p2.id; });

		// map clusters
		int rMaxCid = maxCid(R);
		auto mappings = getClusterMappings(LM, RM, rMaxCid + 1);
		for (Point& p : L) if (mappings[Part::L].find(p.cid) != mappings[Part::L].end()) p.cid = mappings[Part::L][p.cid];
		for (Point& p : R) if (mappings[Part::R].find(p.cid) != mappings[Part::R].end()) p.cid = mappings[Part::R][p.cid];

		// assign clusters
		for (Point& p : L) if (p[divDim] <= bound - 0.5*eps) data[p.id].cid = p.cid;
		for (Point& p : R) if (p[divDim] > bound - 0.5*eps)	data[p.id].cid = p.cid;
		
		// fix clusters shift
		fixClustersShift(data);

        TP("Finished merging");
    }

	void fixClustersShift(std::vector<Point>& data) {
		int maxCid = std::max_element(data.begin(), data.end(), [](const Point& p1, const Point& p2) -> bool {return p1.cid < p2.cid; })->cid;
		std::vector<bool> cidExists(maxCid + 1, false);
		for (Point &p : data)
			cidExists[p.cid] = true;

		std::vector<int> steps;
		for (int cid = 1; cid < cidExists.size(); ++cid) if (!cidExists[cid]) steps.push_back(cid);

		for (Point& p : data)
			if (!steps.empty() && p.cid > steps.front()) {
				int shift = 1;
				for (; shift < steps.size() && p.cid > steps[shift]; ++shift);
				p.cid -= shift;
			}
	}

    void performDbscan(std::vector<Point>& data) {
        std::vector<int> idMap(data.size());
        for(int i = 0; i < data.size(); ++i) {
            idMap[i] = data[i].id;
            data[i].id = i;
        }
		if (divs > 1) {
			Qscan qscan(&data, eps, mi, n, divs - 1);
			qscan.compute();
		} else {
			PLDataSet dataSet(&data, { measures::MeasureId::Euclidean, n });
			Dbscan<PLDataSet> dbscan(&dataSet, { eps, mi });
			dbscan.compute();
		}
		
        for(int i = 0; i < data.size(); ++i) data[i].id = idMap[i];
    }
};
