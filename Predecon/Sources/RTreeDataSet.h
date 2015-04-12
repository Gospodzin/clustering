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
	std::vector<int> rDims;
	std::vector<double> deviations;
	std::vector<int> sortedAttr;

	struct Page {
		std::vector<Page*> adjacent;
		std::vector<Point*> points;
	};

	void emplacePoint(void**& subRTree, int* pagePath, int* pagesCounts, int dimId, Point& p) {
		if(dimId == dims - 1) {
			if(subRTree == NULL) subRTree = (void**)new Page[pagesCounts[dimId]]();
			((Page*)subRTree)[pagePath[dimId]].points.emplace_back(&p);
		}
		else {
			if(subRTree == NULL) subRTree = new void*[pagesCounts[dimId]]();
			emplacePoint((void**&)subRTree[pagePath[dimId]], pagePath, pagesCounts, dimId + 1, p);
		}
	}

	void fillAdjacentPages(void** subRTree, int* pagesCounts, int dimId, std::vector<int>& pagePath) {
		if(dimId == dims - 1) {
			for(int dimPage = 0; dimPage < pagesCounts[dimId]; ++dimPage) {
				pagePath[dimId] = dimPage;
				Page& page = ((Page*)subRTree)[dimPage];
				if(!page.points.empty())
					addAdjacent(rTree, page, pagePath, 0);
			}
		}
		else {
			for(int dimPage = 0; dimPage < pagesCounts[dimId]; ++dimPage) {
				if(subRTree[dimPage] != NULL) {
					pagePath[dimId] = dimPage;
					fillAdjacentPages((void**)subRTree[dimPage], pagesCounts, dimId + 1, pagePath);
				}
			}
		}
	}

	void addAdjacent(void** subRTree, Page& page, std::vector<int>& pagePath, int dimId) {
		if(dimId == dims - 1) {
			if(pagePath[dimId] - 1 >= 0) { Page& adjPage = ((Page*)subRTree)[pagePath[dimId] - 1]; if(!adjPage.points.empty()) page.adjacent.emplace_back(&adjPage); }
			{ Page& adjPage = ((Page*)subRTree)[pagePath[dimId]]; if(!adjPage.points.empty() && &adjPage != &page) page.adjacent.emplace_back(&adjPage); }
			if(pagePath[dimId] + 1 < pagesCounts[dimId]) { Page& adjPage = ((Page*)subRTree)[pagePath[dimId] + 1]; if(!adjPage.points.empty()) page.adjacent.emplace_back(&adjPage); }
		}
		else {
			if(pagePath[dimId] - 1 >= 0 && subRTree[pagePath[dimId] - 1] != NULL) addAdjacent((void**)subRTree[pagePath[dimId] - 1], page, pagePath, dimId + 1);
			if(subRTree[pagePath[dimId]] != NULL) addAdjacent((void**)subRTree[pagePath[dimId]], page, pagePath, dimId + 1);
			if(pagePath[dimId] + 1 < pagesCounts[dimId] && subRTree[pagePath[dimId] + 1] != NULL) addAdjacent((void**)subRTree[pagePath[dimId] + 1], page, pagePath, dimId + 1);
		}
	}

	void calcDeviations() {
		LOG("Calc deviations...");
		TS();
		std::vector<double> means(data->front().size());
		for(Point& p : *data)
			std::transform(p.begin(), p.end(), means.begin(), means.begin(), std::plus<double>());
		std::transform(means.begin(), means.end(), means.begin(), [&](double m) -> double { return m / data->size(); });

		deviations.resize(data->front().size());
		for(Point& p : *data)
			for(int i = 0; i < p.size(); ++i)
				deviations[i] += std::abs(p[i] - means[i]);
		sortedAttr.resize(dimensions());
		for(int i = 0; i < dimensions(); ++i) sortedAttr[i] = i;
		std::sort(sortedAttr.begin(), sortedAttr.end(), [&](int a, int b) -> bool { return deviations[a]>deviations[b]; });
		TP();
	}

	void sortPages(void** subRTree, int* pagesCounts, int dimId, std::vector<int>& pagePath) {
		if(dimId == dims - 1) {
			for(int dimPage = 0; dimPage < pagesCounts[dimId]; ++dimPage) {
				pagePath[dimId] = dimPage;
				Page& page = ((Page*)subRTree)[dimPage];
				if(!page.points.empty())
					std::sort(page.points.begin(), page.points.end(), [&](Point* p1, Point* p2) -> bool { return (*p1)[sortedAttr[0]] < (*p2)[sortedAttr[0]]; });
			}
		}
		else {
			for(int dimPage = 0; dimPage < pagesCounts[dimId]; ++dimPage) {
				if(subRTree[dimPage] != NULL) {
					pagePath[dimId] = dimPage;
					sortPages((void**)subRTree[dimPage], pagesCounts, dimId + 1, pagePath);
				}
			}
		}
	}

public:
	RTreeDataSet(std::vector<Point>* data, measures::MeasureId measureId, double eps) : DataSet(data, measureId) {
		LOG("Creating RTree...");
		TS();
		// init;
		calcDeviations();
		int n = 3 > dimensions() ? dimensions() : 3;
		for(int i=0; i < n; ++i) rDims.push_back(sortedAttr[i]);
		dims = rDims.size();

		min = referenceSelectors::min(*data);
		max = referenceSelectors::max(*data);

		pagesCounts = new int[dims];
		for(int dimId = 0; dimId < dims; ++dimId) pagesCounts[dimId] = (max[rDims[dimId]] - min[rDims[dimId]]) / eps + 1;

		int* pagePath = new int[dims];
		for(Point& p : *data) {
			for(int dimId = 0; dimId < dims; ++dimId)
				pagePath[dimId] = (p[rDims[dimId]] - min[rDims[dimId]]) / eps;

			emplacePoint(rTree, pagePath, pagesCounts, 0, p);
		}

		LOG("Filling adjacent pages...");
		TS();
		std::vector<int> vPagePath(dims);
		fillAdjacentPages(rTree, pagesCounts, 0, vPagePath);
		TP();

		LOG("Sorting points in pages...");
		TS();
		std::vector<int> vPagePath2(dims);
		sortPages(rTree, pagesCounts, 0, vPagePath2);
		TP();

		TP();
	}

	std::vector<Point*> regionQuery(const Point& target, const double& eps, const std::vector<int>& attrs = {}) const {
		std::vector<Point*> neighbours;

		void** pagePtr = rTree;
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

