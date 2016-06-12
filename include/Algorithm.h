#pragma once

#include <vector>
#include "DataSet.h"

class Algorithm {
public:
	struct Params {
		Params() {}
		Params(double eps, int mi) : eps(eps), mi(mi) {}
		Params(double eps, int mi, std::vector<int> attrs) : eps(eps), mi(mi), attrs(attrs) {}
		Params(double eps, int mi, double delta, int lambda, double kappa = 1000.0) : eps(eps), mi(mi), delta(delta), lambda(lambda), kappa(kappa) {}
		Params(double eps, int mi, DataSet::Params dataSetParams, bool odc = false) : eps(eps), mi(mi), dataSetParams(dataSetParams), odc(odc) {}

		double eps;
		unsigned mi;
		double delta;
		int lambda;
		double kappa;
		DataSet::Params dataSetParams;
		bool odc;
		std::vector<int> attrs;
	};
};