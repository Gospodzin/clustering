#pragma once
#include <fstream>
#include <string>
#include "BasicDataSet.h"

class DataWriter
{
public:
	DataWriter(std::string filePath);
	~DataWriter();
	void write(BasicDataSet& data);
private:
	std::ofstream file;
};

