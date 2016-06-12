#pragma once
#include <numeric>
#include <algorithm>

#include "DataSet.h"
#include "referenceSelectors.h"

struct MTIDataSet : DataSet {
private:
	struct PointWithDistances {
		PointWithDistances(Point& point, std::vector<double> distances) : point(point), distances(distances) {}

		Point point;
		std::vector<double> distances;
	};

	std::vector<PointWithDistances> sortedData;
	std::vector<int> idToSortedId;
	std::vector<Point> references;
	std::vector<int> attrs;
	int n;

public:
	MTIDataSet(std::vector<Point>* data, Params params) : n(params.n), attrs(params.attrs), DataSet(data, params) {
		TS("Creating MTIDataSet...");
		// init
		Point max = referenceSelectors::max(*data);
		for(int i = 0; i < n; ++i) { 
			Point r(dimensions(), -1);
			r[i] = max[i];
			references.emplace_back(r); 
		}
		measures::AttrsMeasure measure = measures::getAttrsMeasure(measureId);
		sortedData.reserve(data->size());
		for(Point& p : *data) { 
			std::vector<double> distances(n);
			for(int i = 0; i < n; ++i) distances.emplace_back(this->measure(p, references[i]));
			sortedData.emplace_back(p, distances);
		}
		// sort data
		std::sort(sortedData.begin(), sortedData.end(), [&](const PointWithDistances& p1, const PointWithDistances& p2) -> bool {return p1.distances[0] < p2.distances[0]; });
		// create id to sortedId mapping
		idToSortedId.resize(data->size());
		for(int i = 0; i < (int)sortedData.size(); i++) idToSortedId[sortedData[i].point.id] = i;
		TP("MTIDataSet created");
	}

	std::vector<Point*> regionQuery(const Point& target, const double& eps, const std::vector<int>& attrs = {}) const {
		std::vector<Point*> neighbours;
		int sortedId = idToSortedId[target.id];

		//search upwards
		for(int i = sortedId; i >= 0 && abs(sortedData[sortedId].distances[0] - sortedData[i].distances[0]) <= eps; --i) {
			bool cond = true;
			for(int j = 1; j < n; ++j)
				if(abs(sortedData[sortedId].distances[j] - sortedData[i].distances[j]) > eps) {
					cond = false;
					break;
				}
			if(distance(target, sortedData[i].point, attrs) <= eps)
				neighbours.push_back(&data->at(sortedData[i].point.id));
		}

		//search downwards
		for(int i = sortedId + 1; i < (int)data->size() && abs(sortedData[sortedId].distances[0] - sortedData[i].distances[0]) <= eps; ++i) {
			bool cond = true;
			for(int j = 1; j < n; ++j) 
				if(abs(sortedData[sortedId].distances[j] - sortedData[i].distances[j]) > eps) {
					cond = false; 
					break;
				}
			if(cond && distance(target, sortedData[i].point, attrs) <= eps)
				neighbours.push_back(&data->at(sortedData[i].point.id));
		}

		return neighbours;
	}
};

