#include <numeric>
#include <chrono>
#include <fstream>
#include <cmath>

#include "datasets.h"
#include "algorithms.h"
#include "AlgorithmTester.h"

AlgorithmTester::AlgorithmTester(Data& data) : data(data) {}

std::vector<TestData> AlgorithmTester::testPredecon(TestParams testParams) {
		std::vector<TestData> testData;
		TestParams tp = testParams;

		Data& samples = getSamples(tp.n);
		for (double cur = tp.from; cur < tp.to; cur += tp.step) {
			switch (tp.testVar) {
                        case N:			tp.n = (int)cur;	break;
			case EPS:		tp.eps = cur;		break;
                        case MI:		tp.mi = (int)cur;	break;
			case DELTA:		tp.delta = cur;		break;
                        case LAMBDA:            tp.lambda = (int)cur;   break;
			}

			if (tp.adjustMi && tp.testVar == N) // adjust mi
                tp.mi = testParams.mi * (int)((double)tp.n / tp.from);

			if (tp.testVar == N) samples = getSamples(tp.n);
			else for (Point& p : samples) p.cid = NONE;

			long start = clock();
			if (tp.testDs == TiDS) {
				TIDataSet dataSet = TIDataSet(&samples, DataSet::Params(tp.measureId, referenceSelectors::max));
				Predecon<TIDataSet>(&dataSet, {tp.eps, tp.mi, tp.delta, tp.lambda}).compute();
			}
			else if (tp.testDs == BasicDS) {
				BasicDataSet dataSet = BasicDataSet(&samples, {tp.measureId});
				Predecon<BasicDataSet>(&dataSet, {tp.eps, tp.mi, tp.delta, tp.lambda}).compute();
			}
			double time = double(clock() - start) / CLOCKS_PER_SEC;
			std::map <int, int> clusters;
			for (Point& point : samples)
				if (point.cid != NOISE) {
					if (point.cid == NONE) throw - 1;
					if (clusters.find(point.cid) == clusters.end()) clusters.emplace(point.cid, 1);
					else ++clusters[point.cid];
				}

			int pointsInClusters = 0;
			for (auto cluster : clusters)
				pointsInClusters += cluster.second;
			double sizeAvg = clusters.empty() ? 0 : (double)pointsInClusters / clusters.size();
			double sizeStd = 0;
			for (auto cluster : clusters)
				sizeStd += (cluster.second - sizeAvg)*(cluster.second - sizeAvg);
			sizeStd = std::sqrt(clusters.empty() ? 0 : sizeStd/clusters.size());
			testData.push_back(TestData(tp.n, tp.eps, tp.mi, tp.delta, tp.lambda, time, clusters.size(), sizeAvg, sizeStd));
		}

		return testData;
	}

void AlgorithmTester::writeTestData(std::string path, std::vector<TestData>& testDatas) {
    TS("Writing test data to file...");
	std::ofstream file(path);
	file << TestData::labels() << std::endl;
	for (auto& testData : testDatas)
		file << testData.str() << std::endl;
	file.close();
    TP("Test data written");
}


Data& AlgorithmTester::getSamples(int n) {
	std::vector<int> selectIds = getUniqueRandom(n, 0, data.size());

	samples.clear();
	samples.reserve(n);
	std::for_each(selectIds.begin(), selectIds.end(), [&](int id) -> void{ samples.push_back(data[id]); });
	for (int i = 0; i < samples.size(); ++i)
		samples[i].id = i;

	return samples;
}

std::vector<int> AlgorithmTester::getUniqueRandom(unsigned n, int from, int to) {
	std::vector<int> v(to - from);
	std::iota(v.begin(), v.end(), from); // fill with sequential numbers

	for (size_t l = v.size() - 1; v.size() - l <= n; --l)
		std::swap(v[l], v[l==0 ? 0 : rand() % l]);

	return std::vector<int>(v.end() - n, v.end());
}

