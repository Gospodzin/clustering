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

std::string DataWriter::write(const std::map < Subspace, Clusters >& clustersBySubspace) {
	std::stringstream ss;
	ss << "[Subspace]" << std::endl;
	ss << "  [Cluster] : [Coords]" << std::endl;
    for (auto clusters : clustersBySubspace) {
        ss << write(clusters.first) << std::endl;
        for (auto cluster : clusters.second) {
            for (Point* p : cluster.second->points) {
                ss << "  " << cluster.first << " : "<< p->toString() << std::endl;
            }
        }
    }

	return ss.str();
}

std::string DataWriter::write(const double& duration) {
	std::stringstream ss;
	ss << "Duration [s]: " << duration;
	return ss.str();
}

std::string DataWriter::writeStats(const std::map< Subspace, Clusters >& clustersBySubspace) {
	std::stringstream ss;
	ss << "[Subspace]" << std::endl;
	ss << "  " << "[Cluster]" << " : " << "[Size]" << std::endl;
	for(auto clusters : clustersBySubspace) {
		ss << write(clusters.first) << std::endl;
		for (auto cluster : clusters.second)
			ss << "  " << cluster.first << " : " << cluster.second->points.size() << std::endl;
	}
	return ss.str();
}

void DataWriter::writeData(std::vector<Point>& data) {
	for(Point& p : data)
		file << p.toString() << std::endl;
}