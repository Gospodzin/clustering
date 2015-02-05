#include "DataWriter.h"
#include "Logging/logging.h"
#include <stdlib.h>


DataWriter::DataWriter(std::string filePath) : file(filePath) {
}


DataWriter::~DataWriter() {
	if(file.is_open()) file.close();
}

void DataWriter::write(std::vector<Point>* data) {
	LOG("Writing data to file...");
	TS();
	for (Point& p : *data)
		file << p.toString() << std::endl;
	TP();
}

void DataWriter::writeClusterIds(std::vector<Point>* data) {
	LOG("Writing cluster ids to file...");
	TS();
	for (Point& p : *data)
		file << p.cid << std::endl;
	TP();
}

void DataWriter::writeClusters(std::vector < std::map < std::vector <int>, std::vector<Cluster*> > > clustersByDim) {
	LOG("Writing cluster ids to file...");
	TS();
	for (int i = 0; i < clustersByDim.size(); i++) {
		file << "Dim "<<(i+1) << std::endl;
		std::map < std::vector <int>, std::vector<Cluster*> > clustersByAttr = clustersByDim[i];
		for (auto clusters : clustersByAttr) {
			file << "\t";
			for (int a : clusters.first) file<< (a+1) << " ";
			file<<std::endl;
			for (auto cluster : clusters.second) {
				for (Point* p : cluster->points) {
					file << "\t\t" << cluster->cid << " "<< p->toString() << std::endl;
				}
			}
		}
		

	}
	TP();
}