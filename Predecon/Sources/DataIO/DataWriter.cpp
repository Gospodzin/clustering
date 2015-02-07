#include "DataWriter.h"
#include "Logging/logging.h"
#include <stdlib.h>
#include <sstream>
#include <algorithm>


DataWriter::DataWriter(std::string filePath) : file(filePath) {
}


DataWriter::~DataWriter() {
	if(file.is_open()) file.close();
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

void DataWriter::writeClusterIds(std::vector<Point>* data) {
	LOG("Writing cluster ids to file...");
	TS();
	for (Point& p : *data)
		file << p.cid << std::endl;
	TP();
}

std::string DataWriter::write(const std::vector<int>& subspace) {
	std::stringstream ss;
	ss << "<" << subspace[0];
	std::for_each(subspace.begin() + 1, subspace.end(), [&](int attr)->void{ss << " " << attr; });
    ss << ">";
    return ss.str();
}

std::string DataWriter::write(const std::map < std::vector <int>, std::vector<Cluster*> >& clustersBySubspace) {
	std::stringstream ss;
	ss << "[Subspace]" << std::endl;
	ss << "  [Cluster] : [Coords]" << std::endl;
    for (auto clusters : clustersBySubspace) {
        for (int a : clusters.first) ss<< a << " ";
        ss<<std::endl;
        for (auto cluster : clusters.second) {
            for (Point* p : cluster->points) {
                ss << "  " << cluster->cid << " : "<< p->toString() << std::endl;
            }
        }
    }

	return ss.str();
}
