#pragma once
#include "Dbscan.h"

template<typename T>
class Qscan {
public:
	Qscan(std::vector<Point>* data, double eps, int mi) : data(data), eps(eps), mi(mi) {}

	std::vector<Point>* data;
	const double eps;
	const int mi;
	std::map < Subspace, Clusters > clustersBySubspace;


	void compute() {
		clustersBySubspace = qscan();
	}

	std::map < Subspace, Clusters > getClusters() {
		return clustersBySubspace;
	}

	void clean() {
		for(Point& p : data) p.cid = NONE;
	}


private:
	std::map < Subspace, Clusters > qscan() {
		std::map < Subspace, Clusters > clustersBySubspace;
	}

	int selectDivDim() {
		std::vector<double> means(data->front().size());
		for(Point& p : *data)
			std::transform(p.begin(), p.end(), means.begin(), means.begin(), std::plus<double>());
		std::transform(means.begin(), means.end(), means.begin(), [&](double m) -> double { return m / data->size(); });

		std::vector<double> deviations(data->front().size());
		for(Point& p : *data)
			for(int i = 0; i < p.size(); ++i)
				deviations[i] += std::abs(p[i]-means[i]);

		auto maxIt = std::max_element(deviations.begin(), deviations.end());

		int dim = std::distance(deviations.begin(), maxIt);

		return dim;
	}

	std::vector< std::vector<Point>* > divide() {
		std::vector<Point>* L = new std::vector<Point>(), R = new std::vector<Point>();
		L->reserve((data->size() + 1) / 2);
		R->reserve(data->size() / 2);

		int divDim = selectDivDim();
		
		std::vector<Point*> data;
		data.reserve(this->data->size());
		for(Point& p : *this->data) data.push_back(&p);

		std::nth_element(data.begin(), data.begin() + (data.size() - 1) / 2, data.end(), [&](const Point& p1, const Point& p2) -> bool {return p1[dim] < p2[dim]; });

		for(int i = 0; i <= (data.size() - 1) / 2; ++i) L->push_back(*data[i]);
		for(int i = (data.size() - 1) / 2 + 1; i < data.size(); ++i) R->push_back(*data[i]);

		return {L, R};
	}

	void merge(std::vector<Point>* L, std::vector<Point>* R) {
		std::vector<Point>* MF = new std::vector<Point>();

		for(Point& p : *data)
			MF->push_back(p);

		
	}
};
