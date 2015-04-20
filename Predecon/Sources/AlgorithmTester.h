#pragma once

#include "Data.h"
#include "measures.h"
#include <sstream>

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

	void writeTestData(std::string path, std::vector<TestData>& testDatas);


private:
	Data& data;
	Data samples;

	/* Selects n samples from data */
	Data& getSamples(int n);

	/* Selects n unique random numbers from range <from, to) */
	std::vector<int> getUniqueRandom(unsigned n, int from, int to);


};
