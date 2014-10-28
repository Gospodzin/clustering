#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include "Point.h"
#include "DataSet.h"
#include "measures.h"
#include <cmath>

struct TIDataSet
{
	const double k = 1000.0;

	std::shared_ptr<DataSet> dataSet;
	std::vector<double> distances;
	Point relative;
	measures::Measure measure;
	std::vector<Point*> sortedData;
	std::vector<std::vector<Point*> > neighbourhoods;
	std::vector<std::vector<Point*> > prefNeighbourhoods;
	std::vector<std::vector<double> > allVariances;
	std::vector<int> pDims;
	std::vector<std::vector<double> > prefVectors;
	double eps;
	int mi;
	double delta;
	int lambda;

	TIDataSet(std::shared_ptr<DataSet> dataSet, Point relative, double eps, int mi, double delta, int lambda) : 
		dataSet(dataSet), 
		distances(dataSet->size()),
		relative(relative),
		measure(measures::euclideanDistance),
		sortedData(dataSet->size()),
		neighbourhoods(dataSet->size()),
		prefNeighbourhoods(dataSet->size()),
		allVariances(dataSet->size()),
		pDims(dataSet->size()),
		prefVectors(dataSet->size()),
		eps(eps), mi(mi),
		delta(delta), lambda(lambda) {
		calcDistances();
		sort();
		calcNeighbourhoods(eps);
		calcAllVariances();
		calcPDims();
		calcPrefVectors();
		initialComputation();
	}

	std::vector<Point*> regionQuery(int pointOrderId)
	{
		std::vector<Point*> neighbours;
		Point& selected = *sortedData[pointOrderId];

		//search upwards
		for (int i = pointOrderId; i >= 0 && abs(distances[selected.id] - distances[sortedData[i]->id]) <= eps; --i) {
			Point& p = *sortedData[i];
			if (measure(selected, p) <= eps)
				neighbours.push_back(&p);
		}

		//search downwards
		for (int i = pointOrderId + 1; i < dataSet->size() && abs(distances[selected.id] - distances[sortedData[i]->id]) <= eps; ++i) {
			Point& p = *sortedData[i];
			if (measure(selected, p) <= eps)
				neighbours.push_back(&p);
		}

		return neighbours;
	}

	inline Point& operator[](int n)
	{
		return *sortedData[n];
	}

	inline std::vector<Point*>::iterator begin()
	{
		return sortedData.begin();
	}

	inline std::vector<Point*>::iterator end()
	{
		return sortedData.end();
	}

	inline int size() {
		return dataSet->size();
	}

private:
	void calcDistances() {
		std::for_each(dataSet->begin(), dataSet->end(), [&](const Point& p) -> void{distances[p.id] = measure(p, relative); });
	}
	
	void sort() {
		for (int i = 0; i < size(); ++i) sortedData[i] = &dataSet->at(i);
		std::sort(sortedData.begin(), sortedData.end(), [&](const Point* p1, const Point* p2) -> bool {return distances[p1->id] < distances[p2->id]; });
	}

	void calcNeighbourhoods(double eps) {
		for (int i = 0; i < dataSet->size(); ++i) {
			neighbourhoods[sortedData[i]->id] = regionQuery(i);
		}
	}
	
	void calcAllVariances() {
		std::for_each(dataSet->begin(), dataSet->end(), [&](const Point& p)->void{allVariances[p.id] = calcVariances(p); });
	}

	std::vector<double> calcVariances(const Point& p) {
		std::vector<Point*>& neighbourhood = neighbourhoods[p.id];
		std::vector<double> variances(p.size());
		auto accumVariances = 
			[&](const Point* pp) -> void {	
			for (int i = 0; i < p.size(); ++i) {
				double diff = p[i] - pp->at(i);
				variances[i] += diff*diff;
			}
		};
		std::for_each(neighbourhood.begin(), neighbourhood.end(), accumVariances);
		std::transform(variances.begin(), variances.end(), variances.begin(), [&](double d) -> double{return d / neighbourhood.size(); });
		return variances;
	}

	void calcPDims() {
		for (int i = 0; i < allVariances.size(); ++i) {
			int& pDim = pDims[i];
			auto& variances = allVariances[i];
			std::for_each(variances.begin(), variances.end(), [&](const double& v) -> void{if (v <= delta) ++pDim; });
		}
	}

	void calcPrefVectors() {
		for (int i = 0; i < allVariances.size(); ++i) {
			auto& prefVector = prefVectors[i];
			auto& variances = allVariances[i];
			prefVector.resize(variances.size());
			for (int i = 0; i < variances.size(); ++i)
				prefVector[i] = variances[i] <= delta ? k : 1;
		}
	}

	void initialComputation() {
		auto satisfiesMi = [&](std::vector<Point*>& n) -> bool{return n.size() >= mi; };
		auto satisfiesLambda = [&](Point& p) -> bool{return pDims[p.id] <= lambda; };

		for (int i = 0; i < dataSet->size(); ++i) {
			Point& point = dataSet->at(i);
			if (!satisfiesMi(neighbourhoods[point.id]) || !satisfiesLambda(point))
				point.cid = NOISE;
			else {
				calcPrefNeighbourhood(point);
				auto& prefNeighbourhood = prefNeighbourhoods[point.id];
				if (!satisfiesMi(prefNeighbourhood))
					point.cid = NOISE;
			}
		}
	}

	void calcPrefNeighbourhood(Point& p) {
		auto& neighbourhood = neighbourhoods[p.id];
		auto& prefNeighbourhood = prefNeighbourhoods[p.id];
		for (int i = 0; i < neighbourhood.size(); ++i) {
			Point* neighbour = neighbourhood[i];
			if (generalPrefMeasure(p, *neighbour) <= eps)
				prefNeighbourhood.push_back(neighbour);
		}
	}

	double generalPrefMeasure(Point& p1, Point& p2) {
		return std::max(prefMeasure(p1, p2), prefMeasure(p2, p1));
	}

	double prefMeasure(Point& p1, Point& p2) {
		std::vector<double>& prefVector = prefVectors[p1.id];
		double res = 0;
		for (int i = 0; i < p1.size(); ++i) {
			double diff = p1[i] - p2[i];
			res += prefVector[i] * diff * diff;
		}
		return std::sqrt(res);
	}
};

