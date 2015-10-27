#pragma once
#include <numeric>
#include <algorithm>
#include <vector>

#include "DataSet.h"
#include "referenceSelectors.h"

struct PLDataSet : DataSet {
private:
	std::vector<Point> sortedData;
	std::vector<int> idToSortedId;
    std::vector<int> sortedAttr;
    int sortDim;
	int n;

    void calcDeviations() {
		std::vector<double> means(data->front().size());
		for(Point& p : *data)
			std::transform(p.begin(), p.end(), means.begin(), means.begin(), std::plus<double>());
		std::transform(means.begin(), means.end(), means.begin(), [&](double m) -> double { return m / data->size(); });

		std::vector<double> deviations;
		deviations.resize(data->front().size());
		for(Point& p : *data)
			for(int i = 0; i < p.size(); ++i)
				deviations[i] += std::abs(p[i] - means[i]);
		sortedAttr.resize(dimensions());
		for(int i = 0; i < dimensions(); ++i) sortedAttr[i] = i;
		std::sort(sortedAttr.begin(), sortedAttr.end(), [&](int a, int b) -> bool { return deviations[a]>deviations[b]; });

	}

public:
    PLDataSet(std::vector<Point>* data, Params params) : DataSet(data, params), idToSortedId(data->size()), n(params.n > dimensions() ? dimensions() : params.n) {
        TS("Creating PLDataSet...");
        // init;
		if (!params.dims.empty()) {
			sortedAttr = params.dims;
			n = params.dims.size();
		}
		else {
			calcDeviations();
		}
        this->sortDim = sortedAttr[0];
		LOG("Selected sort dim: " + std::to_string(this->sortDim));
		measures::AttrsMeasure measure = measures::getAttrsMeasure(measureId);
		sortedData.reserve(data->size());
		for(Point& p : *data) sortedData.emplace_back(p);
		// sort data
		TS("Sorting...");
		std::sort(sortedData.begin(), sortedData.end(), [&](const Point& p1, const Point& p2) -> bool {return p1[sortDim] < p2[sortDim]; });
		TP("Sorted");
		// create id to sortedId mapping
		for(int i = 0; i < (int)sortedData.size(); i++) idToSortedId[sortedData[i].id] = i;
        TP("PLDataSet created");
	}

	std::vector<Point*> regionQuery(const Point& target, const double& eps, const std::vector<int>& attrs = {}) const {
		std::vector<Point*> neighbours;
		int sortedId = idToSortedId[target.id];

		//search upwards
		for(int i = sortedId; i >= 0 && abs(sortedData[sortedId][sortDim] - sortedData[i][sortDim]) <= eps; --i) {
            bool candidate = true;
            for(int j = 1; j < n; ++j) if(abs(sortedData[sortedId][sortedAttr[j]] - sortedData[i][sortedAttr[j]]) > eps) {candidate = false; break;}
            if(candidate && distance(target, sortedData[i], attrs) <= eps)
                neighbours.push_back(&data->at(sortedData[i].id));
        }

		//search downwards
		for(int i = sortedId + 1; i < data->size() && abs(sortedData[sortedId][sortDim] - sortedData[i][sortDim]) <= eps; ++i) {
			bool candidate = true;
            for(int j = 1; j < n; ++j) if(abs(sortedData[sortedId][sortedAttr[j]] - sortedData[i][sortedAttr[j]]) > eps) {candidate = false; break;}
			if(candidate && distance(target, sortedData[i], attrs) <= eps)
				neighbours.push_back(&data->at(sortedData[i].id));
		}

		return neighbours;
	}
};

