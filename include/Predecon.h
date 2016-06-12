#pragma once
#include <stack>
#include "logging.h"
#include "Algorithm.h"

template <typename T>
class Predecon : public Algorithm {
public:
    Predecon(T* dataSet, Params params) :
        dataSet(dataSet), prefMeasure(measures::getPrefMeasure(dataSet->measureId)), eps(params.eps), mi(params.mi), delta(params.delta), lambda(params.lambda), kappa(params.kappa),
        curCid(NOISE), neighbourhoods(dataSet->size()), prefNeighbourhoods(dataSet->size()),
        allVariances(dataSet->size()), prefDimsCount(dataSet->size()), prefDims(dataSet->size()), prefVectors(dataSet->size()) {}

	void compute() {
        TS("Performing Predecon...");
		init();
		predecon();
        TP("Predecon performed");
	}

	std::map < Subspace, Clusters > getClusters() {
		std::map < Subspace, Clusters > clustersBySubspace;

        Subspace subspace = utils::attrsFromData(dataSet->data);

		Clusters clusters = utils::dataToClusters(dataSet->data);

		clustersBySubspace.emplace(subspace, clusters);

		return clustersBySubspace;
	}

	void clean() {
        for(Point& p : *dataSet) p.cid = NONE;
	}

    T* const dataSet;
	measures::PrefMeasure prefMeasure;
	const double eps;
	const unsigned mi;
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
        for(int i = 0; i < dataSet->size(); ++i) {
            if((*dataSet)[i].cid == NONE) {
				std::stack<Point*> seeds;
                Point& core = (*dataSet)[i];
				core.cid = ++curCid;
				seeds.push(&core);
				while(!seeds.empty()) {
					Point& point = *seeds.top();
					seeds.pop();
					auto& prefNeighbourhood = prefNeighbourhoods[point.id];
					for(Point* p : prefNeighbourhood) {
						if(p->cid == NONE) {
							p->cid = curCid;
							seeds.push(p);
						}
						else if(prefDimsCount[p->id] <= lambda)
							p->cid = curCid;
					}
				}
			}
		}
	}

	void calcNeighbourhoods() {
        TS("Calculating neihbourhoods...");
        for(int i = 0; i < dataSet->size(); ++i) {
            neighbourhoods[i] = dataSet->regionQuery((*dataSet)[i], eps);
        }
        TP("Neihbourhoods calculated");
	}

	void calcAllVariances() {
        TS("Calculating variances...");
        std::for_each(dataSet->begin(), dataSet->end(), [&](const Point& p)->void {allVariances[p.id] = calcVariances(p); });
        TP("Variances calculated");
	}

	std::vector<double> calcVariances(const Point& p) {
		std::vector<Point*>& neighbourhood = neighbourhoods[p.id];
		std::vector<double> variances(p.size());
		auto accumVariances =
			[&](const Point* pp) -> void {
			for(size_t i = 0; i < p.size(); ++i) {
				double diff = p[i] - pp->at(i);
				variances[i] += diff*diff;
			}
		};
		std::for_each(neighbourhood.begin(), neighbourhood.end(), accumVariances);
		std::transform(variances.begin(), variances.end(), variances.begin(), [&](double d) -> double {return d / neighbourhood.size(); });
		return variances;
	}

	void calcPrefDims() {
        TS("Calculating preference dims...");
        for(size_t i = 0; i < allVariances.size(); ++i) {
            auto& variances = allVariances[i];
            for(size_t j = 0; j < variances.size(); ++j)
                if(variances[j] <= delta) {
                    ++prefDimsCount[i];
                    prefDims[i].push_back(j);
                }
        }
        TP("Preference dims calculated");
	}

	void calcPrefVectors() {
        TS("Calculating preference vectors...");
        for(size_t i = 0; i < allVariances.size(); ++i) {
            auto& prefVector = prefVectors[i];
            auto& variances = allVariances[i];
            prefVector.resize(variances.size());
            for(size_t i = 0; i < variances.size(); ++i)
                prefVector[i] = variances[i] <= delta ? kappa : 1;
        }
        TP("Preference vectors calculated");
	}

	void initialComputation() {
        TS("Marking noise points and calculatng preference neighbourhoods...");
			auto satisfiesMi = [&](std::vector<Point*>& n) -> bool {return n.size() >= mi; };
		auto satisfiesLambda = [&](Point& p) -> bool {return prefDimsCount[p.id] <= lambda; };

        for(int i = 0; i < dataSet->size(); ++i) {
            Point& point = (*dataSet)[i];
			if(!satisfiesMi(neighbourhoods[point.id]) || !satisfiesLambda(point))
				point.cid = NOISE;
			else {
				calcPrefNeighbourhood(point);
				auto& prefNeighbourhood = prefNeighbourhoods[point.id];
				if(!satisfiesMi(prefNeighbourhood))
					point.cid = NOISE;
			}
		}
        TP("Noise points marked and pref neighbourhoods calculated");
	}

	void calcPrefNeighbourhood(Point& p) {
		auto& neighbourhood = neighbourhoods[p.id];
		auto& prefNeighbourhood = prefNeighbourhoods[p.id];
		for(size_t i = 0; i < neighbourhood.size(); ++i) {
			Point* neighbour = neighbourhood[i];
			if(generalPrefMeasure(p, *neighbour) <= eps)
				prefNeighbourhood.push_back(neighbour);
		}
	}

	double generalPrefMeasure(Point& p1, Point& p2) {
		return std::max(prefMeasure(p1, p2, prefVectors[p1.id]), prefMeasure(p2, p1, prefVectors[p2.id]));
	}
};
