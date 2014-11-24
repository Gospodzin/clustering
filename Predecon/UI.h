#pragma once
#include <iostream>
#include <string>
#include "dataio.h"
#include "clustering.h"
#include "StatsCollector.h"
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

class UI {
public:
	void run() {
		std::cout << "Enter data file path: ";
		std::string filePath;
		std::cin >> filePath;
		std::cout << "Choose measure: \n1. Euclidean\n2. Manhattan\n";
		int measureId;
		std::cin >> measureId;
		measures::Measure measure;
		measures::PrefMeasure prefMeasure;
		if (measureId == 1) {
			measure = measures::euclideanDistanceSquared;
			prefMeasure = measures::euclideanDistanceSquared;
		}
		else {
			measure = measures::manhattanDistance;
			prefMeasure = measures::manhattanDistance;
		}
		std::cout << "Choose algorithm: \n1. TIPredecon\n2. BasicPredecon\n";
		int algorithmId;
		std::cin >> algorithmId;

		std::cout << "eps: ";
		double eps;
		std::cin >> eps;
		std::cout << "mi: ";
		int mi;
		std::cin >> mi;
		std::cout << "delta: ";
		double delta;
		std::cin >> delta;
		std::cout << "lambda: ";
		int lambda;
		std::cin >> lambda;

		if (measureId == 1) eps *= eps;

		if (algorithmId == 1)
			performTIPredecon(filePath, measure, prefMeasure, eps, mi, delta, lambda);
		else
			performTIPredecon(filePath, measure, prefMeasure, eps, mi, delta, lambda);
	}

private:
	void performTIPredecon(std::string filePath, measures::Measure measure, measures::PrefMeasure prefMeasure, double eps, int mi, double delta, int lambda) {
		std::vector<std::string> sfileName = split(filePath, '\\');
		std::string fileName = sfileName[sfileName.size()-1];

		std::vector<Point>* data = DataLoader(filePath).load();
		clock_t t = clock();
		TIDataSet dataSet(data, measure, referenceSelectors::max);
		Predecon<TIDataSet> predecon(&dataSet, prefMeasure, eps, mi, delta, lambda);
		predecon.compute();
		double time = double((clock() - t)) / CLOCKS_PER_SEC;
		char outPath[1000];
		sprintf_s(outPath, "%f_%d_%f_%d_%s", sqrt(eps), mi, delta, lambda, fileName);
		DataWriter("out_"+std::string(outPath)).writeClusterIds(data);
		StatsCollector sc;
		sc.collect(*data);
		sc.write("stat_" + std::string(outPath), time);
	}
	void performBasicPredecon(std::string filePath, measures::Measure measure, measures::PrefMeasure prefMeasure, double eps, int mi, double delta, int lambda) {
		std::vector<Point>* data = DataLoader(filePath).load();
		BasicDataSet dataSet(data, measure);
		Predecon<BasicDataSet> predecon(&dataSet, prefMeasure, eps, mi, delta, lambda);
		predecon.compute();
	}
};

