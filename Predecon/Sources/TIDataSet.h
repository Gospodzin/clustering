#pragma once
#include <numeric>
#include <algorithm>

#include "DataSet.h"
#include "referenceSelectors.h"

struct TIDataSet : DataSet
{
private:
	struct PointWithDistance {
		PointWithDistance(Point& point, double distance) : point(point), distance(distance) {}

		Point point;
		double distance;
	};

	std::vector<PointWithDistance> sortedData;
	std::vector<int> idToSortedId;
    Point reference;
	std::vector<int> attrs;

public:
	TIDataSet(std::vector<Point>* data, Params params) : attrs(params.attrs), DataSet(data, params) {
        TS("Creating TIDataSet...");
		// init
		reference = params.referenceSelector == NULL ? params.reference : params.referenceSelector(*data);
		measures::AttrsMeasure measure = measures::getAttrsMeasure(measureId);
		sortedData.reserve(data->size());
		for (Point& p : *data) sortedData.emplace_back(p, attrs.size() == 0 ? this->measure(p, reference) : measure(p, reference, attrs));
		// sort data
		std::sort(sortedData.begin(), sortedData.end(), [&](const PointWithDistance& p1, const PointWithDistance& p2) -> bool {return p1.distance < p2.distance; });
		// create id to sortedId mapping
		idToSortedId.resize(data->size());
		for (int i = 0; i < (int)sortedData.size(); i++) idToSortedId[sortedData[i].point.id] = i;
        TP("TIDataSet created");
	}
	 
    std::vector<Point*> regionQuery(const Point& target, const double& eps, const std::vector<int>& attrs = {}) const {
        std::vector<Point*> neighbours;
        int sortedId = idToSortedId[target.id];

		//search upwards
		for (int i = sortedId; i >= 0 && abs(sortedData[sortedId].distance - sortedData[i].distance) <= eps; --i) {
            if (distance(target, sortedData[i].point, attrs) <= eps)
                neighbours.push_back(&data->at(sortedData[i].point.id));
		}

		//search downwards
		for (int i = sortedId + 1; i < (int)data->size() && abs(sortedData[sortedId].distance - sortedData[i].distance) <= eps; ++i) {
            if (distance(target, sortedData[i].point, attrs) <= eps)
                neighbours.push_back(&data->at(sortedData[i].point.id));
        }
		
		return neighbours;
	}
};

