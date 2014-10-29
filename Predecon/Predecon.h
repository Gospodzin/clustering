#pragma once
#include <stack>

template <typename T>
class Predecon
{
public:
	Predecon(T* data, measures::Measure measure, double eps, int mi, double delta, int lambda, double k = 1000.0) :
		data(data), measure(measure), eps(eps), mi(mi), delta(delta), lambda(lambda),	k(k),
		curCid(NOISE), neighbourhoods(data->size()), prefNeighbourhoods(data->size()),
		allVariances(data->size()),	pDims(data->size()), prefVectors(data->size()) {
		calcNeighbourhoods();
		calcAllVariances();
		calcPDims();
		calcPrefVectors();
		initialComputation();
	}
	
	void compute() {
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
						else if (pDims[p->id] <= lambda)
							p->cid = curCid;
					}
				}
			}
		}
	}

	T* const data;
	const measures::Measure measure;
	const double eps;
	const int mi;
	const double delta;
	const int lambda;
	const double k;

	int curCid;
	std::vector<std::vector<Point*> > neighbourhoods;
	std::vector<std::vector<double> > allVariances;
	std::vector<int> pDims;
	std::vector<std::vector<double> > prefVectors;
	std::vector<std::vector<Point*> > prefNeighbourhoods;

private:
	void calcNeighbourhoods() {
		for (int i = 0; i < data->size(); ++i) {
			neighbourhoods[i] = data->regionQuery((*data)[i], eps, measure);
		}
	}

	void calcAllVariances() {
		std::for_each(data->begin(), data->end(), [&](const Point& p)->void {allVariances[p.id] = calcVariances(p); });
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

	void calcPDims() {
		for (int i = 0; i < allVariances.size(); ++i) {
			int& pDim = pDims[i];
			auto& variances = allVariances[i];
			std::for_each(variances.begin(), variances.end(), [&](const double& v) -> void {if (v <= delta) ++pDim; });
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
		auto satisfiesMi = [&](std::vector<Point*>& n) -> bool {return n.size() >= mi; };
		auto satisfiesLambda = [&](Point& p) -> bool {return pDims[p.id] <= lambda; };

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