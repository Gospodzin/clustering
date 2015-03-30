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
    std::vector<double> deviations;
    std::vector<int> sortedAttr;
	Point reference;
    int sortDim;
	int n;

    void calcDeviations() {
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

	}

public:
    PLDataSet(std::vector<Point>* data, measures::MeasureId measureId, int n, std::vector<int> attrs = {}) :
        DataSet(data, measureId), idToSortedId(data->size()), n(n) {
        LOG("Creating PLDataSet...");
        TS();
        // init;
        calcDeviations();
        this->sortDim = sortedAttr[0];
		measures::AttrsMeasure measure = measures::getAttrsMeasure(measureId);
		sortedData.reserve(data->size());
		for(Point& p : *data) sortedData.emplace_back(p);
		// sort data
		std::sort(sortedData.begin(), sortedData.end(), [&](const Point& p1, const Point& p2) -> bool {return p1[sortDim] < p2[sortDim]; });
		// create id to sortedId mapping
		for(int i = 0; i < (int)sortedData.size(); i++) idToSortedId[sortedData[i].id] = i;
		TP()
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
private:
	void testPoint(Point& p, std::vector<Point>& neighbours) {

	}
};

