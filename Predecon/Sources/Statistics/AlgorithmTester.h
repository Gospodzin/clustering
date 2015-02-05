#pragma once
#include <numeric>
#include "Clustering/clustering.h"

typedef std::vector<Point> Data;

struct TestData {
	int n; // no. of samples
	long time; // time in ms
};

class AlgorithmTester {
public:
	AlgorithmTester(Data& data) : data(data) {}

	/* test algorithm for number of samples in range <from, to) with given step  */
	void test(int from, int to, int step) {
		for (int n = from; n < to; n += step) {
			Data samples = getSamples(n);
			BasicDataSet dataSet = BasicDataSet(&samples, measures::euclideanDistance);

		}
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