#pragma once
#include <map>
#include <numeric>
#include <chrono>
#include <algorithm>
#include "Cluster.h"
#include "Subspace.h"
#include "Data.h"
#include "libpca\pca.h"
#include "logging.h"
#include "measures.h"
#include "referenceSelectors.h"

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

	static std::vector<double> calcMedians(std::vector<Point>& data) {
		std::vector<double> medians(data.front().size());

		std::vector<Point*> dataCpy;
		dataCpy.reserve(data.size());
		for (Point& p : data) dataCpy.push_back(&p);

		for (int dim = 0; dim < data.front().size(); ++dim) {
			std::nth_element(dataCpy.begin(), dataCpy.begin() + (dataCpy.size() - 1) / 2, dataCpy.end(), [&](const Point* p1, const Point* p2) -> bool {return p1->at(dim) < p2->at(dim); });
			double median = dataCpy[dataCpy.size() / 2]->at(dim);
            medians[dim] = median;
		}

		return medians;
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

	static std::vector<int> getRandomSequence(size_t n) {
		std::vector<int> ids(n);
		std::iota(ids.begin(), ids.end(), 0);
		std::random_shuffle(ids.begin(), ids.end());

		return ids;
	}

	static Data* randomSample(Data& data, size_t size, size_t n, int seed = time(NULL)) {
		srand(seed);
		Data* sample = new Data();
		std::vector<int> dims = getRandomSequence(data.dimensions());
		std::vector<int> ids = getRandomSequence(data.size());
		std::for_each(ids.begin(), ids.begin() + size, [&](int id) -> void {
			Point p(n);
			for (int dimOffset = 0; dimOffset < n; ++dimOffset) {
                p[dimOffset] = data[id][dims[dimOffset]];
			}
			sample->emplace_back(p); 
		});

		std::vector<std::string> headers(n);
		for (int dimOffset = 0; dimOffset < n; ++dimOffset) {
            headers[dimOffset] = data.headers[dims[dimOffset]];
		}
		sample->headers = headers;

        for(int id = 0; id < sample->size(); ++id) {
            sample->at(id).id = id;
        }

		return sample;
	}

	static Data* randomSampleNoDim(Data& data, size_t size, int seed = time(NULL)) {
		srand(seed);
		Data* sample = new Data();
		std::vector<int> ids = getRandomSequence(data.size());
		std::for_each(ids.begin(), ids.begin() + size, [&](int id) -> void {
			sample->emplace_back(data[id]);
		});

		for (int id = 0; id < sample->size(); ++id) {
			sample->at(id).id = id;
		}

		return sample;
	}

	static void defaultHeaders(Data& data) {
		for (int dim = 0; dim < data.dimensions(); ++dim) data.headers.emplace_back(std::to_string(dim));
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

	static Data* pca(std::vector<Point>& data, int dims = -1) {
        TS("Performing PCA...");
		stats::pca pca(data.front().size());
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

	static double length(Point& p) {
		double length = 0;
		for (int i = 0; i < p.size(); ++i) {
			length += p[i] * p[i];
		}

		return std::sqrt(length);
	}
	
	static void normalize(std::vector<Point>& data) {
		for (int i = 0; i < data.size(); ++i) {
			double pointLength = length(data[i]);
			if (pointLength != 0.0) {
				for (int j = 0; j < data.front().size(); ++j) {
					data[i][j] /= pointLength;
				}
			} else {
				LOG("Cannot normalize 0 length vector, id: " + std::to_string(data[i].id));
			}
		}
	}

	static Data* normalized(std::vector<Point>& data) {
		TS("Normalization...");
		Data* output = new Data(data.size(), Point(data.front().size(), -1));
		for (int i = 0; i < data.size(); ++i)
			output->headers.emplace_back("N"+std::to_string(i));
		for (int i = 0; i < data.size(); ++i) {
			Point& p = (*output)[i] = data[i];
			double pointLength = length(p);
			if (pointLength != 0.0) {
				for (int j = 0; j < p.size(); ++j) {
					p[j] /= pointLength;
				}
			}
			else {
				LOG("Cannot normalize 0 length vector, id: " + std::to_string(data[i].id));
			}
		}
		TP("Normalization finished.");
		return output;
	}

	static Data* dimNormalized(std::vector<Point>& data) {
		TS("Normalization...");
		Point minP = referenceSelectors::min(data);
		Point maxP = referenceSelectors::max(data);
		std::vector<double> width(data.front().size());
		std::vector<double> shift(data.front().size());
		for (int i = 0; i < width.size(); ++i) width[i] = maxP[i] - minP[i], shift[i] = -(maxP[i]+minP[i])/2;
		Data* output = new Data(data.size(), Point(data.front().size(), -1));
		for (int i = 0; i < data.size(); ++i) output->headers.emplace_back("N" + std::to_string(i));
		for (int i = 0; i < data.size(); ++i) {
			Point& p = (*output)[i] = data[i];
			for (int j = 0; j < data.front().size(); ++j) {
				if (width[j] != 0.0) {
					p[j] = (p[j] + shift[j]) / width[j];
				} 
				else {
					LOG("Cannot normalize 0 width dimension, dim: " + std::to_string(j));
				}
			}
		}
		TP("Normalization finished.");
		return output;
	}
	
	static Data* removeNoise(std::vector<Point>& data) {
		TS("Removing noise...");
		Data* output = new Data();
		for (int i = 0; i < data.size(); ++i) output->headers.emplace_back(std::to_string(i));
		for (int i = 0; i < data.size(); ++i) if(data[i].cid != NOISE) output->emplace_back(data[i]);
		for (int i = 0; i < output->size(); ++i) output->at(i).id = i;
		TP("Noise removed.");
		return output;
	}

	static double calcM(Point& r, std::vector<Point>& data, measures::Measure measure){
		double min = measure(*std::min_element(data.begin(), data.end(), [&](Point& p1, Point& p2){return measure(p1, r) < measure(p2, r); }), r);
		double max = measure(*std::max_element(data.begin(), data.end(), [&](Point& p1, Point& p2){return measure(p1, r) < measure(p2, r); }), r);
		return max - min;
	};

	static double calcMAD(Point& r, std::vector<Point>& data, measures::Measure measure){
		std::vector<Point> aux;
		aux.reserve(data.size());
		for (Point& p : data) {
			aux.emplace_back(std::vector<double>(1, measure(r, p)), -1);
		}
		return calcMeanDeviations(aux)[0];
	};

	static double calcSD(Point& r, std::vector<Point>& data, measures::Measure measure){
		std::vector<Point> aux;
		aux.reserve(data.size());
		for (Point& p : data) {
			aux.emplace_back(std::vector<double>(1, measure(r, p)), -1);
		}
		return calcStandardDeviations(aux)[0];
	};
}
