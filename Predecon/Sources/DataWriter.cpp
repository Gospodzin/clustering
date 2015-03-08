#include "DataWriter.h"
#include "logging.h"
#include <stdlib.h>
#include <sstream>
#include <algorithm>


DataWriter::DataWriter(std::string filePath) : file(filePath) {
}


DataWriter::~DataWriter() {
	if(file.is_open()) file.close();
}

void DataWriter::writeGraph(std::vector<Point>* data) {
	for (Point& p : *data)
		file << p.cid << " " << p.toString() << std::endl;
}

std::string DataWriter::write(std::vector<Point>* data) {
	std::stringstream ss;
	ss << "[Cluster] : [Coords]" << std::endl;
	for (Point& p : *data)
        ss << p.cid << " : " << p.toString() << std::endl;

	return ss.str();
}

void DataWriter::write(std::string str) {
	LOG("Writing data to file...");
	TS();
	file << str;
	TP();
}

std::string DataWriter::write(std::vector<Point>* data, std::vector<std::vector<int>>& prefDims) {
	std::stringstream ss;
	ss << "[Cluster] : [Coords] : [Pref Dims]" << std::endl;
	for (size_t i = 0; i < data->size(); ++i) {
		Point& p = (*data)[i];
        ss << p.cid << " : " << p.toString() << " : ";
		for (int prefDim : prefDims[i])
			ss << prefDim << " ";
        ss << std::endl;
	}
	return ss.str();
}

std::string DataWriter::write(const Subspace& subspace) {
	std::stringstream ss;
	ss << "<" << subspace[0];
	std::for_each(subspace.begin() + 1, subspace.end(), [&](int attr)->void{ss << " " << attr; });
    ss << ">";
    return ss.str();
}

std::string DataWriter::write(const std::map < Subspace, std::vector<Cluster*> >& clustersBySubspace) {
	std::stringstream ss;
	ss << "[Subspace]" << std::endl;
	ss << "  [Cluster] : [Coords]" << std::endl;
    for (auto clusters : clustersBySubspace) {
        ss << write(clusters.first) << std::endl;
        for (auto cluster : clusters.second) {
            for (Point* p : cluster->points) {
                ss << "  " << cluster->cid << " : "<< p->toString() << std::endl;
            }
        }
    }

	return ss.str();
}

std::string DataWriter::write(const std::map<CId, int>& sizeByCId) {
	std::stringstream ss;
	ss << "No. of clusters found: " << (sizeByCId.find(NOISE) == sizeByCId.end() ? sizeByCId.size() : sizeByCId.size() - 1) << std::endl;
	ss << "No. of noise points: " << (sizeByCId.find(NOISE) == sizeByCId.end() ? 0 : sizeByCId.at(NOISE)) << std::endl;
	ss << "No. of points per cluster: " << std::endl;
	ss << "[Cluster] : [Size]" << std::endl;
	for(auto& size : sizeByCId)
		ss << size.first << " : " << size.second << std::endl;
	return ss.str();
}

std::string DataWriter::write(const double& duration) {
	std::stringstream ss;
	ss << "Duration [s]: " << duration << std::endl;
	return ss.str();
}

std::string DataWriter::write(const std::map<Subspace, std::map<CId, int> >& clusterSizesBySubspace) {
	std::stringstream ss;
	ss << "[Subspace]" << std::endl;
	ss << "  " << "[Cluster]" << " : " << "[Size]" << std::endl;
	for (auto& clusterSizes : clusterSizesBySubspace) {
		ss << write(clusterSizes.first) << std::endl;
		for (auto& clusterSize : clusterSizes.second)
			ss << "  " << clusterSize.first << " : " << clusterSize.second << std::endl;
	}
	return ss.str();
}
