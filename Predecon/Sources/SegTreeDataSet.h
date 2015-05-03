#pragma once
#include <numeric>
#include <algorithm>
#include <vector>

#include "Data.h"
#include "DataSet.h"
#include "referenceSelectors.h"
#include "utils.h"

struct SegTreeDataSet : DataSet {
private:
	Point min;
	Point max;
	void** SegTree = NULL;
	int dims;
	double eps;
	int* pagesCounts;
	std::vector<int> rDims;
	std::vector<double> deviations;
	std::vector<int> sortedAttr;

	struct Page {
		std::vector<Page*> adjacent;
		std::vector<Point*> points;
	};

	void emplacePoint(void**& subSegTree, int* pagePath, int* pagesCounts, int dimId, Point& p) {
		if(dimId == dims - 1) {
			if(subSegTree == NULL) subSegTree = (void**)new Page[pagesCounts[dimId]]();
			((Page*)subSegTree)[pagePath[dimId]].points.emplace_back(&p);
		}
		else {
			if(subSegTree == NULL) subSegTree = new void*[pagesCounts[dimId]]();
			emplacePoint((void**&)subSegTree[pagePath[dimId]], pagePath, pagesCounts, dimId + 1, p);
		}
	}

	void fillAdjacentPages(void** subSegTree, int* pagesCounts, int dimId, std::vector<int>& pagePath) {
		if(dimId == dims - 1) {
			for(int dimPage = 0; dimPage < pagesCounts[dimId]; ++dimPage) {
				pagePath[dimId] = dimPage;
				Page& page = ((Page*)subSegTree)[dimPage];
				if(!page.points.empty())
					addAdjacent(SegTree, page, pagePath, 0);
			}
		}
		else {
			for(int dimPage = 0; dimPage < pagesCounts[dimId]; ++dimPage) {
				if(subSegTree[dimPage] != NULL) {
					pagePath[dimId] = dimPage;
					fillAdjacentPages((void**)subSegTree[dimPage], pagesCounts, dimId + 1, pagePath);
				}
			}
		}
	}

	void addAdjacent(void** subSegTree, Page& page, std::vector<int>& pagePath, int dimId) {
		if(dimId == dims - 1) {
			if(pagePath[dimId] - 1 >= 0) { Page& adjPage = ((Page*)subSegTree)[pagePath[dimId] - 1]; if(!adjPage.points.empty()) page.adjacent.emplace_back(&adjPage); }
			{ Page& adjPage = ((Page*)subSegTree)[pagePath[dimId]]; if(!adjPage.points.empty() && &adjPage != &page) page.adjacent.emplace_back(&adjPage); }
			if(pagePath[dimId] + 1 < pagesCounts[dimId]) { Page& adjPage = ((Page*)subSegTree)[pagePath[dimId] + 1]; if(!adjPage.points.empty()) page.adjacent.emplace_back(&adjPage); }
		}
		else {
			if(pagePath[dimId] - 1 >= 0 && subSegTree[pagePath[dimId] - 1] != NULL) addAdjacent((void**)subSegTree[pagePath[dimId] - 1], page, pagePath, dimId + 1);
			if(subSegTree[pagePath[dimId]] != NULL) addAdjacent((void**)subSegTree[pagePath[dimId]], page, pagePath, dimId + 1);
			if(pagePath[dimId] + 1 < pagesCounts[dimId] && subSegTree[pagePath[dimId] + 1] != NULL) addAdjacent((void**)subSegTree[pagePath[dimId] + 1], page, pagePath, dimId + 1);
		}
	}

	void calcDeviations() {
        TS("Calc deviations...");
		deviations = utils::calcMeanDeviations(*data);
		sortedAttr.resize(dimensions());
		for(int i = 0; i < dimensions(); ++i) sortedAttr[i] = i;
		std::sort(sortedAttr.begin(), sortedAttr.end(), [&](int a, int b) -> bool { return deviations[a]>deviations[b]; });
        TP("Deviations calculated");
	}

	void sortPages(void** subSegTree, int* pagesCounts, int dimId, std::vector<int>& pagePath) {
		if(dimId == dims - 1) {
			for(int dimPage = 0; dimPage < pagesCounts[dimId]; ++dimPage) {
				pagePath[dimId] = dimPage;
				Page& page = ((Page*)subSegTree)[dimPage];
				if(!page.points.empty())
					std::sort(page.points.begin(), page.points.end(), [&](Point* p1, Point* p2) -> bool { return (*p1)[sortedAttr[0]] < (*p2)[sortedAttr[0]]; });
			}
		}
		else {
			for(int dimPage = 0; dimPage < pagesCounts[dimId]; ++dimPage) {
				if(subSegTree[dimPage] != NULL) {
					pagePath[dimId] = dimPage;
					sortPages((void**)subSegTree[dimPage], pagesCounts, dimId + 1, pagePath);
				}
			}
		}
	}

public:
    SegTreeDataSet(std::vector<Point>* data, Params params) : DataSet(data, params), dims(params.n > dimensions() ? dimensions() : params.n), eps(params.eps) {
        TS("Creating SegTree...");
		// init;
		calcDeviations();
        for(int i=0; i < dims; ++i) rDims.emplace_back(sortedAttr[i]);

		min = referenceSelectors::min(*data);
		max = referenceSelectors::max(*data);

		pagesCounts = new int[dims];
		for(int dimId = 0; dimId < dims; ++dimId) pagesCounts[dimId] = (max[rDims[dimId]] - min[rDims[dimId]]) / eps + 1;

		int* pagePath = new int[dims];
		for(Point& p : *data) {
			for(int dimId = 0; dimId < dims; ++dimId)
				pagePath[dimId] = (p[rDims[dimId]] - min[rDims[dimId]]) / eps;

			emplacePoint(SegTree, pagePath, pagesCounts, 0, p);
		}

        TS("Filling adjacent pages...");
		std::vector<int> vPagePath(dims);
		fillAdjacentPages(SegTree, pagesCounts, 0, vPagePath);
        TP("Adjacent pages filled");

        TS("Sorting points in pages...");
		std::vector<int> vPagePath2(dims);
		sortPages(SegTree, pagesCounts, 0, vPagePath2);
        TP("Points in pages sorted");

        TP("SegTree created");
	}

	std::vector<Point*> regionQuery(const Point& target, const double& eps, const std::vector<int>& attrs = {}) const {
		std::vector<Point*> neighbours;

		void** pagePtr = SegTree;
		for(int dimId = 0; dimId < dims - 1; ++dimId) { // go down the tree to the bottom level
			int dimPage = (target[rDims[dimId]] - min[rDims[dimId]]) / eps;
			pagePtr = (void**)pagePtr[dimPage];
		}
		
		int dimPage = (target[rDims[dims - 1]] - min[rDims[dims - 1]]) / eps;
		Page& page = ((Page*)pagePtr)[dimPage];
		
		for(Point* p : page.points)
			if(distance(target, *p, {}) <= eps)
				neighbours.emplace_back(p);

		for(Page* adjPage : page.adjacent) {
			int dimPage = (target[rDims[0]] - min[rDims[0]]) / eps;
			Point& adjP = *adjPage->points.front();
			int pDimPage = (adjP[rDims[0]] - min[rDims[0]]) / eps;
			if(pDimPage > dimPage) 
				for(auto it = adjPage->points.begin(); it != adjPage->points.end(); ++it) {
					if(std::abs(target[rDims[0]] - (**it)[rDims[0]]) > eps) break;
					if(distance(target, **it, {}) <= eps)
						neighbours.emplace_back(*it);
				}
			else if(pDimPage < dimPage)
				for(auto it = adjPage->points.rbegin(); it != adjPage->points.rend(); ++it) {
					if(std::abs(target[rDims[0]] - (**it)[rDims[0]]) > eps) break;
					if(distance(target, **it, {}) <= eps)
						neighbours.emplace_back(*it);
				}
			else 
				for(Point* p : adjPage->points)
					if(distance(target, *p, {}) <= eps)
						neighbours.emplace_back(p);
			
		}

		return neighbours;
	}

};

