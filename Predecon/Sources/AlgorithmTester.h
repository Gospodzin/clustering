#pragma once

#include "Data.h"
#include "measures.h"
#include <sstream>
#include "Algorithm.h"

enum TestVar {
	N,
    EPS,
	MI,
	DELTA,
	LAMBDA
};

enum TestDs {
	TiDS,
	BasicDS
};

struct TestParams {
	TestParams(){}

	TestVar testVar;
	TestDs testDs;
	double from, to, step;
	int n, mi, lambda;
	double eps, delta;
	measures::MeasureId measureId;
	bool adjustMi;

	std::string str() {
		std::stringstream ss;
		ss << dStr() << mStr() << "_";
		switch (testVar){
		case N:			ss << "n=" << rStr()	<< "_e=" << eps		<< "_m" << (adjustMi ? "~" : "") << "=" << mi		<< "_d=" << delta	<< "_l=" << lambda;		break;
		case EPS:		ss << "n=" << n			<< "_e=" << rStr()	<< "_m" << (adjustMi ? "~" : "") << "=" << mi		<< "_d=" << delta	<< "_l=" << lambda;		break;
		case MI:		ss << "n=" << n			<< "_e=" << eps		<< "_m" << (adjustMi ? "~" : "") << "=" << rStr()	<< "_d=" << delta	<< "_l=" << lambda;		break;
		case DELTA:		ss << "n=" << n			<< "_e=" << eps		<< "_m" << (adjustMi ? "~" : "") << "=" << mi		<< "_d=" << rStr()	<< "_l=" << lambda;		break;
		case LAMBDA:	ss << "n=" << n			<< "_e=" << eps		<< "_m" << (adjustMi ? "~" : "") << "=" << mi		<< "_d=" << delta	<< "_l=" << rStr();		break;
		}
		return ss.str();
	}

private:
	std::string dStr() {
		switch (testDs) {
		case TiDS: return "T";
		case BasicDS: return "B";
		}
	}

	std::string mStr() {
		switch (measureId) {
		case measures::MeasureId::Euclidean: return "E";
		case measures::MeasureId::Manhattan: return "M";
		}
	}

	std::string rStr() {
		std::stringstream ss;
		ss << from << "-" << step << "-" << to;
		return ss.str();
	}
};

struct TestData {
	TestData(int n, double eps, int mi, double delta, int lambda, double time, int clustersNo, double sizeAvg, double sizeStd)
		: n(n), eps(eps), mi(mi), delta(delta), lambda(lambda), time(time), clustersNo(clustersNo), sizeAvg(sizeAvg), sizeStd(sizeStd) {}

	int n, mi, lambda, clustersNo;
	double eps, delta, time, sizeAvg, sizeStd;

	static std::string labels() {
		std::stringstream ss;
		ss << "N" << " " << "EPS" << " " << "MI" << " " << "DELTA" << " " << "LAMBDA" << " " << "TIME" << " " << "CLUSTERS" << " " << "SIZE_AVG" << " " << "SIZE_STD";
		return ss.str();
	}

	std::string str(){
		std::stringstream ss;
		ss << n << " " << eps << " " << mi << " " << delta << " " << lambda << " " << time << " " << clustersNo << " " << sizeAvg << " " << sizeStd;
		return ss.str();
	}
};

class AlgorithmTester {
public:
	AlgorithmTester(Data& data);

	std::vector<TestData> testPredecon(TestParams testParams);

	template<class D, class A>
	std::vector<TestData> test(TestParams testParams) {
		std::vector<TestData> testData;
		TestParams tp = testParams;

		Data& samples = getSamples(tp.n);
		for(double cur = tp.from; cur < tp.to; cur += tp.step) {
			switch(tp.testVar) {
			case N:			tp.n = (int)cur;	break;
			case EPS:		tp.eps = cur;		break;
			case MI:		tp.mi = (int)cur;	break;
			case DELTA:		tp.delta = cur;		break;
			case LAMBDA:            tp.lambda = (int)cur;   break;
			}

			if(tp.adjustMi && tp.testVar == N) // adjust mi
				tp.mi = testParams.mi * (int)((double)tp.n / tp.from);

			if(tp.testVar == N) samples = getSamples(tp.n);
			else for(Point& p : samples) p.cid = NONE;

			long start = clock();
			if(tp.testDs == TiDS) {
				TIDataSet dataSet = TIDataSet(&samples, DataSet::Params(tp.measureId, referenceSelectors::max));
				Predecon<TIDataSet>(&dataSet, {tp.eps, tp.mi, tp.delta, tp.lambda}).compute();
			}
			else if(tp.testDs == BasicDS) {
				BasicDataSet dataSet = BasicDataSet(&samples, {tp.measureId});
				Predecon<BasicDataSet>(&dataSet, {tp.eps, tp.mi, tp.delta, tp.lambda}).compute();
			}
			double time = double(clock() - start) / CLOCKS_PER_SEC;
			std::map <int, int> clusters;
			for(Point& point : samples)
				if(point.cid != NOISE) {
					if(point.cid == NONE) throw - 1;
					if(clusters.find(point.cid) == clusters.end()) clusters.emplace(point.cid, 1);
					else ++clusters[point.cid];
				}

			int pointsInClusters = 0;
			for(auto cluster : clusters)
				pointsInClusters += cluster.second;
			double sizeAvg = clusters.empty() ? 0 : (double)pointsInClusters / clusters.size();
			double sizeStd = 0;
			for(auto cluster : clusters)
				sizeStd += (cluster.second - sizeAvg)*(cluster.second - sizeAvg);
			sizeStd = std::sqrt(clusters.empty() ? 0 : sizeStd / clusters.size());
			testData.push_back(TestData(tp.n, tp.eps, tp.mi, tp.delta, tp.lambda, time, clusters.size(), sizeAvg, sizeStd));
		}

		return testData;
	}
	void writeTestData(std::string path, std::vector<TestData>& testDatas);


private:
	Data& data;
	Data samples;

	/* Selects n samples from data */
	Data& getSamples(int n);

	/* Selects n unique random numbers from range <from, to) */
	std::vector<int> getUniqueRandom(unsigned n, int from, int to);


};
