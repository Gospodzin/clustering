#include "DataWriter.h"
#include "logging.h"


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