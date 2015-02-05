#pragma once
#include <numeric>
#include <chrono>

#include "Clustering/clustering.h"

typedef std::vector<Point> Data;

struct TestData {
	TestData(int n, double time, int clustersNo) {
		this->n = n;
		this->time = time;
		this->clustersNo = clustersNo;
	}

	int n; // no. of samples
	double time; // sec
	int clustersNo;
};

class AlgorithmTester {
public:
	AlgorithmTester(Data& data) : data(data) {}

	

	std::vector<TestData> testNPredecon(int nFrom, int nTo, int nStep, double eps, int mi, double delta, int lambda, measures::MeasureId measureId) {
		std::vector<TestData> testData;
		testData.reserve((nTo - nFrom) / nStep);
		for (int n = nFrom; n < nTo; n += nStep) {
			Data samples = getSamples(n);
			BasicDataSet dataSet = BasicDataSet(&samples, measureId);

			long start = clock();
			Predecon<BasicDataSet>(&dataSet, eps, mi, delta, lambda).compute();
			double time = double(clock() - start) / CLOCKS_PER_SEC;
			std::set <int> clusters;
			for (Point& point : dataSet)
				if (point.cid != NOISE && clusters.find(point.cid) == clusters.end()) 
					clusters.emplace(point.cid);
			testData.push_back(TestData(n, time, clusters.size()));
		}

		return testData;
	}

private:
	Data& data;

	/* Selects n samples from data */
	Data getSamples(int n) {
		std::vector<int> selectIds = getUniqueRandom(n, 0, data.size());
		
		std::vector<Point> samples;
		samples.reserve(n);
		std::for_each(selectIds.begin(), selectIds.end(), [&](int id) -> void{ samples.push_back(data[id]); });

		return samples;
	}

	/* Selects n unique random numbers from range <from, to) */
	std::vector<int> getUniqueRandom(int n, int from, int to) {
		std::vector<int> v(to - from);
		std::iota(v.begin(), v.end(), from); // fill with sequential numbers

		for (int l = v.size() - 1; v.size() - l <= n; --l)
			std::swap(v[l], v[rand() % l]);

		return std::vector<int>(v.end() - n, v.end());
	}

};