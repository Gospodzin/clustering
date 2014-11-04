#pragma once
#include <iostream>
#include <string>
#include "dataio.h"
#include "clustering.h"

class UI {
public:
	void run() {
		std::cout << "Enter data file path: ";
		std::string filePath;
		std::cin >> filePath;
		std::cout << "Choose measure: \n1. EuclideanSquared\n2. Manhattan\n";
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


		if (algorithmId == 1)
			performTIPredecon(filePath, measure, prefMeasure, eps, mi, delta, lambda);
		else
			performTIPredecon(filePath, measure, prefMeasure, eps, mi, delta, lambda);
	}

private:
	void performTIPredecon(std::string filePath, measures::Measure measure, measures::PrefMeasure prefMeasure, double eps, int mi, double delta, int lambda) {
		std::vector<Point>* data = DataLoader(filePath).load();
		TIDataSet dataSet(data, measure, referenceSelectors::max);
		Predecon<TIDataSet> predecon(&dataSet, prefMeasure, eps, mi, delta, lambda);
		predecon.compute();
	}
	void performBasicPredecon(std::string filePath, measures::Measure measure, measures::PrefMeasure prefMeasure, double eps, int mi, double delta, int lambda) {
		std::vector<Point>* data = DataLoader(filePath).load();
		BasicDataSet dataSet(data, measure);
		Predecon<BasicDataSet> predecon(&dataSet, prefMeasure, eps, mi, delta, lambda);
		predecon.compute();
	}
};