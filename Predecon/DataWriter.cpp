#include "DataWriter.h"


DataWriter::DataWriter(std::string filePath) : file(filePath) {
}


DataWriter::~DataWriter() {
	if(file.is_open()) file.close();
}

void DataWriter::write(std::vector<Point>* data) {
	for (Point& p : *data)
		file << p.toString() << std::endl;
}