#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include "Point.h"
#include "BasicDataSet.h"
#include "measures.h"
#include <cmath>

struct TIDataSet : DataSet
{
	Point relative;
	measures::Measure measure;
	std::vector<Point*> sortedData;
	std::vector<double> distances;
	std::vector<int> idToSortedId;

	TIDataSet(std::vector<Point>* data, Point relative, measures::Measure measure) :
		DataSet(data), relative(relative), measure(measure),
		sortedData(data->size()), distances(data->size()), idToSortedId(data->size()) {
		// init
		for (int i = 0; i < size(); ++i) sortedData[i] = &data->at(i);
		// calculate distances
		std::for_each(data->begin(), data->end(), [&](const Point& p) -> void {distances[p.id] = measure(p, relative); });
		// sort data
		std::sort(sortedData.begin(), sortedData.end(), [&](const Point* p1, const Point* p2) -> bool {return distances[p1->id] < distances[p2->id]; });
		// create id to sortedId mapping
		for (int i = 0; i < sortedData.size(); i++) idToSortedId[sortedData[i]->id] = i;
	}

	std::vector<Point*> regionQuery(const Point& target, const double& eps, measures::Measure sortingMeasureIsChosen) {
		std::vector<Point*> neighbours;
		int sortedId = idToSortedId[target.id];

		//search upwards
		for (int i = sortedId; i >= 0 && abs(distances[target.id] - distances[sortedData[i]->id]) <= eps; --i) {
			Point& p = *sortedData[i];
			if (this->measure(target, p) <= eps)
				neighbours.push_back(&p);
		}

		//search downwards
		for (int i = sortedId + 1; i < data->size() && abs(distances[target.id] - distances[sortedData[i]->id]) <= eps; ++i) {
			Point& p = *sortedData[i];
			if (this->measure(target, p) <= eps)
				neighbours.push_back(&p);
		}

		return neighbours;
	}
};

