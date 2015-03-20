#pragma once

#include <vector>
#include "Point.h"
#include "TIDataSet.h"
#include "Dbscan.h"

namespace tests {

#pragma region Auxiliaries
	void DbcanTestXY();
	void DbcanTestX();
	void DbcanTestY();
	void runTests() {
		DbcanTestXY();
		DbcanTestX();
		DbcanTestY();
	}

	std::vector<Point> getTestData() {
		std::vector<Point> data { 
			Point({ 0, 4 }, 0), 
			Point({ 1, 4 }, 1), 
			Point({ 1, 5 }, 2), 
			Point({ 2, 1 }, 3), 
			Point({ 2, 3 }, 4), 
			Point({ 2, 4 }, 5), 
			Point({ 3, 1 }, 6), 
			Point({ 3, 2 }, 7), 
			Point({ 3, 4 }, 8), 
			Point({ 7, 0 }, 9), 
			Point({ 7, 1 }, 10), 
			Point({ 8, 1 }, 11), 
			Point({ 9, 2 }, 12), 
			Point({ 9, 4 }, 13), 
			Point({ 10, 3 }, 14) };

		return data;
	}
#pragma endregion

#pragma region Tests
	void DbcanTestXY() {
		std::vector<Point> data = getTestData();
		TIDataSet dataSet(&data, measures::MeasureId::Euclidean, referenceSelectors::max);
		Dbscan<TIDataSet> dbscan(&dataSet, 1.1, 3);
		Clusters clusters = dbscan.getClusters().begin()->second;

		assert(clusters.size() == 3 && "DbcanTestXY");
	}

	void DbcanTestX() {
		std::vector<Point> data = getTestData();
		TIDataSet dataSet(&data, measures::MeasureId::Euclidean, referenceSelectors::max);
		Dbscan<TIDataSet> dbscan(&dataSet, 1.1, 3, { 0 });
		Clusters clusters = dbscan.getClusters().begin()->second;

		assert(clusters.size() == 2 && "DbcanTestX");
	}

	void DbcanTestY() {
		std::vector<Point> data = getTestData();
		TIDataSet dataSet(&data, measures::MeasureId::Euclidean, referenceSelectors::max);
		Dbscan<TIDataSet> dbscan(&dataSet, 1.1, 3, { 1 });
		Clusters clusters = dbscan.getClusters().begin()->second;

		assert(clusters.size() == 1 && "DbcanTestY");
	}


	
#pragma endregion
}