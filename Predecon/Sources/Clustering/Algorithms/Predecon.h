#pragma once
#include <stack>
#include "Logging/logging.h"

template <typename T>
class Predecon
{
public:
	Predecon(T* data, double eps, int mi, double delta, int lambda, double kappa = 1000.0) :
		data(data), prefMeasure(measures::getPrefMeasure(data->measureId)), eps(eps), mi(mi), delta(delta), lambda(lambda), kappa(kappa),
		curCid(NOISE), neighbourhoods(data->size()), prefNeighbourhoods(data->size()),
		allVariances(data->size()),	prefDimsCount(data->size()), prefDims(data->size()), prefVectors(data->size()) {
		init();
	}
	
	void compute() {
		LOG("Determining clusters...")TS()
		predecon();
		TP()
	}

	T* const data;
	measures::PrefMeasure prefMeasure;
	const double eps;
	const int mi;
	const double delta;
	const int lambda;
	const double kappa;

	int curCid;
	std::vector<std::vector<Point*> > neighbourhoods;
	std::vector<std::vector<double> > allVariances;
	std::vector<int> prefDimsCount;
	std::vector<std::vector<int> > prefDims;
	std::vector<std::vector<double> > prefVectors;
	std::vector<std::vector<Point*> > prefNeighbourhoods;

private:
	void init() {
		calcNeighbourhoods();
		calcAllVariances();
		calcPrefDims();
		calcPrefVectors();
		initialComputation();
	}

	void predecon() {
		for (int i = 0; i < data->size(); ++i) {
			if ((*data)[i].cid == NONE) {
				std::stack<Point*> seeds;
				Point& core = (*data)[i];
				core.cid = ++curCid;
				seeds.push(&core);
				while (!seeds.empty()) {
					Point& point = *seeds.top();
					seeds.pop();
					auto& prefNeighbourhood = prefNeighbourhoods[point.id];
					for (Point* p : prefNeighbourhood) {
						if (p->cid == NONE) {
							p->cid = curCid;
							seeds.push(p);
						}
						else if (prefDimsCount[p->id] <= lambda)
							p->cid = curCid;
					}
				}
			}
		}
	}

	void calcNeighbourhoods() {
		LOG("Calculating neihbourhoods...")TS()
		for (int i = 0; i < data->size(); ++i) {
			neighbourhoods[i] = data->regionQuery((*data)[i], eps);
		}
		TP()
	}

	void calcAllVariances() {
		LOG("Calculating variances...")TS()
		std::for_each(data->begin(), data->end(), [&](const Point& p)->void {allVariances[p.id] = calcVariances(p); });
		TP()
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
		std::transform(variances.begin(), variances.end(), variances.begin(), [&](double d) -> double {return d / neighbourhood.size(); });
		return variances;
	}

	void calcPrefDims() {
		LOG("Calculating preference dimensions...")TS()
		for (int i = 0; i < allVariances.size(); ++i) {
			auto& variances = allVariances[i];
			for (int j = 0; j < variances.size(); ++j)
				if (variances[j] <= delta) { 
					++prefDimsCount[i]; 
					prefDims[i].push_back(j);
				}
		}
		TP()
	}

	void calcPrefVectors() {
		LOG("Calculating preference vectors...")TS()
		for (int i = 0; i < allVariances.size(); ++i) {
			auto& prefVector = prefVectors[i];
			auto& variances = allVariances[i];
			prefVector.resize(variances.size());
			for (int i = 0; i < variances.size(); ++i)
				prefVector[i] = variances[i] <= delta ? kappa : 1;
		}
		TP()
	}

	void initialComputation() {
		LOG("Marking noise points and calculatng preference neighbourhoods...")TS()
		auto satisfiesMi = [&](std::vector<Point*>& n) -> bool {return n.size() >= mi; };
		auto satisfiesLambda = [&](Point& p) -> bool {return prefDimsCount[p.id] <= lambda; };

		for (int i = 0; i < data->size(); ++i) {
			Point& point = (*data)[i];
			if (!satisfiesMi(neighbourhoods[point.id]) || !satisfiesLambda(point))
				point.cid = NOISE;
			else {
				calcPrefNeighbourhood(point);
				auto& prefNeighbourhood = prefNeighbourhoods[point.id];
				if (!satisfiesMi(prefNeighbourhood))
					point.cid = NOISE;
			}
		}
		TP()
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
		return std::max(prefMeasure(p1, p2, prefVectors[p1.id]), prefMeasure(p2, p1, prefVectors[p2.id]));
	}
};