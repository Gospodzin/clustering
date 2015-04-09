#pragma once
#include <numeric>
#include <algorithm>
#include <vector>

#include "DataSet.h"
#include "referenceSelectors.h"

struct RTreeDataSet : DataSet {
private:
	Point min;
	Point max;
	void** rTree = NULL;
	int dims;
	int* pagesCounts;

	void emplacePoint(void**& subRTree, int* pagePath, int* pagesCounts, int dim, Point& p) {
		if(dim == dims - 1) {
			if(subRTree == NULL) subRTree = (void**)new std::vector<Point*>[pagesCounts[dim]];
			if(pagePath[dim] - 1 >= 0)               ((std::vector<Point*>*)subRTree)[pagePath[dim] - 1].emplace_back(&p);
			                                         ((std::vector<Point*>*)subRTree)[pagePath[dim]    ].emplace_back(&p);
			if(pagePath[dim] + 1 < pagesCounts[dim]) ((std::vector<Point*>*)subRTree)[pagePath[dim] + 1].emplace_back(&p);
		}
		else {
			if(subRTree == NULL) subRTree = new void*[pagesCounts[dim]]();
			if(pagePath[dim] - 1 >= 0)               emplacePoint((void**&)subRTree[pagePath[dim] - 1], pagePath, pagesCounts, dim + 1, p);
			                                         emplacePoint((void**&)subRTree[pagePath[dim]    ], pagePath, pagesCounts, dim + 1, p);
			if(pagePath[dim] + 1 < pagesCounts[dim]) emplacePoint((void**&)subRTree[pagePath[dim] + 1], pagePath, pagesCounts, dim + 1, p);
		}
	}

public:
	RTreeDataSet(std::vector<Point>* data, measures::MeasureId measureId, double eps) : DataSet(data, measureId), dims(dimensions()) {
		LOG("Creating RTree...");
		TS();
		// init;
		min = referenceSelectors::min(*data);
		max = referenceSelectors::max(*data);

		pagesCounts = new int[dims];
		for(int i = 0; i < dims; ++i) pagesCounts[i] = std::ceil((max[i] - min[i]) / eps);

		int* pagePath = new int[dims];
		for(Point& p : *data) {
			for(int dim = 0; dim < dims; ++dim)
				pagePath[dim] = (p[dim] - min[dim]) / eps;

			emplacePoint(rTree, pagePath, pagesCounts, 0, p);
		}

		TP()
	}

	std::vector<Point*> regionQuery(const Point& target, const double& eps, const std::vector<int>& attrs = {}) const {
		std::vector<Point*> neighbours;

		void** pagePtr = rTree;
		for(int dim = 0; dim < dims - 1; ++dim) { // go down the tree to the bottom level
			int dimPage = (target[dim] - min[dim]) / eps;
			pagePtr = (void**)pagePtr[dimPage];
		}
		
		int dimPage = (target[dims - 1] - min[dims - 1]) / eps;
		std::vector<Point*>& page = ((std::vector<Point*>*)pagePtr)[dimPage];
		
		for(Point* p : page)
			if(distance(target, *p, {}) <= eps)
				neighbours.emplace_back(p);

		return neighbours;
	}

};

