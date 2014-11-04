#pragma once
#include "DataSet.h"
#include <algorithm>

namespace referenceSelectors {
	typedef  Point(*ReferenceSelector)(std::vector<Point>& data);
	
	Point max(std::vector<Point>& data) {
		Point reference = data[0];
		for (Point& p : data)
			for (int i = 0; i < (int)p.size(); ++i)
				if (p[i] > reference[i])
					reference[i] = p[i];
			
		return reference;
	}
}

struct TIDataSet : DataSet
{
	Point reference;
	std::vector<Point*> sortedData;
	std::vector<double> distances;
	std::vector<int> idToSortedId;

	TIDataSet(std::vector<Point>* data, measures::Measure measure, Point reference) :
		DataSet(data, measure), reference(reference),
		sortedData(data->size()), distances(data->size()), idToSortedId(data->size()) {
		LOG("Creating TIDataSet...")
		TS();
		// init
		for (int i = 0; i < size(); ++i) sortedData[i] = &data->at(i);
		// calculate distances
		std::for_each(data->begin(), data->end(), [&](const Point& p) -> void {distances[p.id] = measure(p, reference); });
		// sort data
		std::sort(sortedData.begin(), sortedData.end(), [&](const Point* p1, const Point* p2) -> bool {return distances[p1->id] < distances[p2->id]; });
		// create id to sortedId mapping
		for (int i = 0; i < (int)sortedData.size(); i++) idToSortedId[sortedData[i]->id] = i;
		TP();
	}

	TIDataSet(std::vector<Point>* data, measures::Measure measure, referenceSelectors::ReferenceSelector referenceSelector) : TIDataSet(data, measure, referenceSelector(*data)) {}

	std::vector<Point*> regionQuery(const Point& target, const double& eps) {
		std::vector<Point*> neighbours;
		int sortedId = idToSortedId[target.id];

		//search upwards
		for (int i = sortedId; i >= 0 && abs(distances[target.id] - distances[sortedData[i]->id]) <= eps; --i) {
			Point& p = *sortedData[i];
			if (measure(target, p) <= eps)
				neighbours.push_back(&p);
		}

		//search downwards
		for (int i = sortedId + 1; i < (int)data->size() && abs(distances[target.id] - distances[sortedData[i]->id]) <= eps; ++i) {
			Point& p = *sortedData[i];
			if (measure(target, p) <= eps)
				neighbours.push_back(&p);
		}

		return neighbours;
	}
};

