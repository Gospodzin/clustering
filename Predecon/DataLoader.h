#pragma once
#include <string>
#include <fstream>
#include <memory>
#include "DataSet.h"

class DataLoader
{
public:
	DataLoader(std::string filePath);
	~DataLoader();
	std::shared_ptr<DataSet> load();
private:
	Point strToPoint(const std::string& line);
	int readDimsCount();
	std::ifstream file;
	int dimsCount;
	int idCarrier;
};

