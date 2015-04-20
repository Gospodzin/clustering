#pragma once
#include <map>
#include <numeric>
#include <chrono>
#include <algorithm>
#include <Eigen/Dense>
#include "Cluster.h"
#include "Subspace.h"
#include "Data.h"
#include "libpca\pca.h"
#include "logging.h"

namespace utils {
	static Clusters dataToClusters(std::vector<Point>* data) {
		Clusters clusters;
		for(Point& p : *data) {
			if(p.cid != NOISE) {
				if(clusters.find(p.cid) != clusters.end()) {
					clusters[p.cid]->points.push_back(&p);
				}
				else {
					clusters.emplace(p.cid, new Cluster(p.cid));
					clusters[p.cid]->points.push_back(&p);
				}
			}
		}

		return clusters;
	}

	static int countNoise(Clusters& clusters, Data* data) {
		int clusteredCount = 0;
		for(auto cluster : clusters) clusteredCount += cluster.second->points.size();

		int noiseCount = data->size() - clusteredCount;

		return noiseCount;
	}

	static Subspace attrsFromData(std::vector<Point>* data) {
		Subspace attrs(data->front().size());
		std::iota(attrs.begin(), attrs.end(), 0);

		return attrs;
	}

	static std::vector<double> calcMeans(std::vector<Point>& data) {
		std::vector<double> means(data.front().size());
	
		for(Point& p : data) std::transform(p.begin(), p.end(), means.begin(), means.begin(), std::plus<double>());
		std::transform(means.begin(), means.end(), means.begin(), [&](double m) -> double { return m / data.size(); });

		return means;
	}

	static std::vector<double> calcMeanDeviations(std::vector<Point>& data) {
		std::vector<double> means = utils::calcMeans(data);
		std::vector<double> deviations(data.front().size());

		for(Point& p : data) for(int dim = 0; dim < p.size(); ++dim) deviations[dim] += std::abs(p[dim] - means[dim]);
		std::transform(deviations.begin(), deviations.end(), deviations.begin(), [&](double d) -> double { return d / data.size(); });

		return deviations;
	}

	static std::vector<double> calcStandardDeviations(std::vector<Point>& data) {
		std::vector<double> means = utils::calcMeans(data);
		std::vector<double> deviations(data.front().size());

		for(Point& p : data) for(int dim = 0; dim < p.size(); ++dim) deviations[dim] += (p[dim] - means[dim])*(p[dim] - means[dim]);
		std::transform(deviations.begin(), deviations.end(), deviations.begin(), [&](double d) -> double { return std::sqrt(d/data.size()); });
		return deviations;
	}

	static std::vector<Point> randomSample(std::vector<Point>& data, size_t n) {
		srand(time(NULL));
		std::vector<Point> sample;
		std::vector<int> ids(data.size());
		std::iota(ids.begin(), ids.end(), 0);
		std::random_shuffle(ids.begin(), ids.end());
		std::for_each(ids.begin(), ids.begin() + n, [&](int id) -> void { sample.emplace_back(data[id]); });
		return sample;
	}

	static void maxVarDir(Data& data) {
		Eigen::MatrixXd mat;
		mat.resize(data.size(), data.dimensions());
		for(int r = 0; r < mat.rows(); ++r)
			for(int c = 0; c < mat.cols(); ++c)
				mat(r, c) = data[r][c];

		Eigen::MatrixXd centered = mat.rowwise() - mat.colwise().mean();
		Eigen::MatrixXd cov = (centered.adjoint() * centered) / double(mat.rows() - 1);

		Eigen::EigenSolver<Eigen::MatrixXd> es(cov);
	}

    static std::vector<Point> pca(std::vector<Point> data) {
        TS("Performing PCA...");
        int dims = data.front().size();
        stats::pca pca(dims);
        for(auto& p : data) pca.add_record(p);
        pca.solve();

        std::vector<Point> output(data.size(), Point(dims, -1));
        for(int i = 0; i < data.size(); ++i) output[i].id = data[i].id;
        for(int dim = 0; dim < dims; ++dim) {
            std::vector<double> principal = pca.get_principal(dim);
            for(int i = 0; i < data.size(); ++i)
                output[i].emplace_back(principal[i]);
        }
        TP("PCA performed");
        return output;
    }

	static Data* pca(Data& data, int dims) {
        TS("Performing PCA...");
		stats::pca pca(data.dimensions());
		for(auto& p : data) pca.add_record(p);
		pca.solve();

        Data* output = new Data(data.size(), Point(dims, -1));
		for(int i = 0; i < data.size(); ++i) (*output)[i].id = data[i].id;
		for(int dim = 0; dim < dims; ++dim) {
            output->headers.emplace_back("PCA" + std::to_string(dim));
			std::vector<double> principal = pca.get_principal(dim);
			for(int i = 0; i < data.size(); ++i)
				(*output)[i].emplace_back(principal[i]);
		}
        TP("PCA performed");
		return output;
	}
}
