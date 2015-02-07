#include <numeric>
#include <chrono>
#include <fstream>

#include "Clustering\DataSets\datasets.h"
#include "Clustering\Algorithms\algorithms.h"
#include "AlgorithmTester.h"

AlgorithmTester::AlgorithmTester(Data& data) : data(data) {}

std::vector<TestData> AlgorithmTester::testPredecon(TestVar testVar, TestDs testDs, double from, double to, double step, int n, double eps, int mi, double delta, int lambda, measures::MeasureId measureId) {
		std::vector<TestData> testData;

		Data& samples = getSamples(n);
		for (double cur = from; cur < to; cur += step) {
			switch (testVar) {
			case N:			n = cur;		break;
			case EPS:		eps = cur;		break;
			case MI:		mi = cur;		break;
			case DELTA:		delta = cur;	break;
			case LAMBDA:	lambda = cur;	break;
			}

			if (testVar == N) samples = getSamples(n);
			long start = clock();
			if (testDs == TiDS) {
				TIDataSet dataSet = TIDataSet(&samples, measureId, referenceSelectors::max);
				Predecon<TIDataSet>(&dataSet, eps, mi, delta, lambda).compute();
			}
			else if (testDs == BasicDS) {
				BasicDataSet dataSet = BasicDataSet(&samples, measureId);
				Predecon<BasicDataSet>(&dataSet, eps, mi, delta, lambda).compute();
			}
			double time = double(clock() - start) / CLOCKS_PER_SEC;
			std::set <int> clusters;
			for (Point& point : samples)
				if (point.cid != NOISE && clusters.find(point.cid) == clusters.end())
					clusters.emplace(point.cid);
			testData.push_back(TestData(cur, time, clusters.size()));
		}

		return testData;
	}

void AlgorithmTester::writeTestData(std::string path, std::vector<TestData>& testDatas) {
	LOG("Writing test data to file...");
	TS();
	std::ofstream file(path);
	file << "VAL" << " " << "TIME" << " " << "CLUSTERS" << std::endl;
	for (auto& testData : testDatas)
		file << testData.testVar << " " << testData.time << " " << testData.clustersNo << std::endl;
	file.close();
	TP();
}


Data& AlgorithmTester::getSamples(int n) {
	std::vector<int> selectIds = getUniqueRandom(n, 0, data.size());

	samples.clear();
	samples.reserve(n);
	std::for_each(selectIds.begin(), selectIds.end(), [&](int id) -> void{ samples.push_back(data[id]); });

	return samples;
}

std::vector<int> AlgorithmTester::getUniqueRandom(unsigned n, int from, int to) {
	std::vector<int> v(to - from);
	std::iota(v.begin(), v.end(), from); // fill with sequential numbers

	for (size_t l = v.size() - 1; v.size() - l <= n; --l)
		std::swap(v[l], v[rand() % l]);

	return std::vector<int>(v.end() - n, v.end());
}

