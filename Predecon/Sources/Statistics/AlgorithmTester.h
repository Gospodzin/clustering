#pragma once

#include "Clustering\Point.h"
#include "Clustering\measures.h"

typedef std::vector<Point> Data;

struct TestData {
	TestData(double testVar, double time, int clustersNo) {
		this->testVar = testVar;
		this->time = time;
		this->clustersNo = clustersNo;
	}

	double testVar; // no. of samples
	double time; // sec
	int clustersNo;
};

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

class AlgorithmTester {
public:
	AlgorithmTester(Data& data);

	std::vector<TestData> testPredecon(TestVar testVar, TestDs testDs, double from, double to, double step, int n, double eps, int mi, double delta, int lambda, measures::MeasureId measureId);

	void writeTestData(std::string path, std::vector<TestData>& testDatas);


private:
	Data& data;
	std::vector<Point> samples;

	/* Selects n samples from data */
	Data& getSamples(int n);

	/* Selects n unique random numbers from range <from, to) */
	std::vector<int> getUniqueRandom(unsigned n, int from, int to);


};