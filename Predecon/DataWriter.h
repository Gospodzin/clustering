#pragma once
#include <fstream>
#include <string>
#include "DataSet.h"

class DataWriter
{
public:
	DataWriter(std::string filePath);
	~DataWriter();
	void write(DataSet& data);
private:
	std::ofstream file;
};

