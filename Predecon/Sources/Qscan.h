#pragma once
#include "Dbscan.h"

class Qscan {
public:
	Qscan(std::vector<Point>* data, double eps, int mi) : data(data), eps(eps), mi(mi) {}

	std::vector<Point>* data;
	const double eps;
	const int mi;
	std::map < Subspace, Clusters > clustersBySubspace;


	void compute() {
        LOG("Performing Qscan...");
        TS();
        qscan();
		LOG("Qscan finished...");
		TP();
	}

    std::map < Subspace, Clusters > getClusters() {
        std::map < Subspace, Clusters > clustersBySubspace;

        Subspace subspace = utils::attrsFromData(data);

        Clusters clusters = utils::dataToClusters(data);

        clustersBySubspace.emplace(subspace, clusters);

        return clustersBySubspace;
    }

	void clean() {
        for(Point& p : *data) p.cid = NONE;
	}


private:
    void qscan() {
		std::map < Subspace, Clusters > clustersBySubspace;

		int divDim = selectDivDim();

		std::vector<Point*> dataCpy;
		dataCpy.reserve(data->size());
		for(Point& p : *data) dataCpy.push_back(&p);

        std::nth_element(dataCpy.begin(), dataCpy.begin() + (dataCpy.size() - 1) / 2, dataCpy.end(), [&](const Point* p1, const Point* p2) -> bool {return p1->at(divDim) < p2->at(divDim); });
        double median = dataCpy[(dataCpy.size() - 1) / 2]->at(divDim);

		std::vector<std::vector<Point>*> division = divide(divDim, median);

        std::vector<Point>* L = division[0];
        std::vector<Point>* R = division[1];

        performDbscan(L);
        performDbscan(R);

        merge(L, R, divDim, median);
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

	std::vector< std::vector<Point>* > divide(int divDim, double bound) {
        std::vector<Point>* L = new std::vector<Point>();
        std::vector<Point>* R = new std::vector<Point>();
		L->reserve((data->size() + 1) / 2);
		R->reserve(data->size() / 2);

		for(Point& p : *data) if(p[divDim] <= bound) L->push_back(p); else R->push_back(p);

		return {L, R};
	}

	void merge(std::vector<Point>* L, std::vector<Point>* R, int divDim, double bound) {
		std::vector<Point>* MF = new std::vector<Point>();

		for(Point& p : *data)
            if(p[divDim] <= bound + 2*eps && p[divDim] >= bound - 2*eps) MF->push_back(p);

        performDbscan(MF);

        int lMaxCid=0;
        for(Point& p : *L) {
            data->at(p.id).cid = p.cid;
            if(p.cid > lMaxCid) lMaxCid = p.cid;
        }

        int rMaxCid=lMaxCid;
        for(Point& p : *R) {
            data->at(p.id).cid = p.cid == NOISE ? p.cid : p.cid + lMaxCid;
            if(p.cid > rMaxCid) rMaxCid = p.cid;
        }

        int mfCidOffset = data->size();
        for(Point& mfp : *MF) mfp.cid += mfp.cid == NOISE ? mfp.cid : mfCidOffset;

        for(Point& mfp : *MF)
			if(mfp.cid != NOISE) {
                Point& dp = data->at(mfp.id);
				if(dp.cid == NOISE)
					dp.cid = mfp.cid;
				else if(dp.cid != mfp.cid)
					for(Point& pp : *data)
						if(pp.cid == dp.cid) pp.cid = mfp.cid;
			}

        std::vector<bool> cidExists(rMaxCid + 1);
        for(Point &p : *data) if(!cidExists[p.cid]) cidExists[p.cid] = true;
        std::vector<int> steps;
        for(int cid = 1; cid < cidExists.size(); ++cid) if(!cidExists[cid]) steps.push_back(cid);
        for(Point& p : *data)
            if(!steps.empty() && p.cid > steps.front() && p.cid <= rMaxCid) {
                int shift = 1;
                for(;shift < steps.size() && p.cid > steps[shift]; ++shift);
                p.cid -= shift;
            }
        for(Point& p : *data) if(p.cid > data->size()) p.cid = rMaxCid + p.cid - mfCidOffset - steps.size();
    }

    void performDbscan(std::vector<Point>* data) {
        std::vector<int> idMap(data->size());
        for(int i = 0; i < data->size(); ++i) {
            idMap[i] = data->at(i).id;
            data->at(i).id = i;
        }
        TIDataSet dataSet(data, measures::MeasureId::Euclidean, referenceSelectors::max);
        Dbscan<TIDataSet> dbscan(&dataSet, eps, mi);
        dbscan.compute();
        for(int i = 0; i < data->size(); ++i) data->at(i).id = idMap[i];
    }
};
