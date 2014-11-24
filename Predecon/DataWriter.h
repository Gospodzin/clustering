#pragma once
#include <fstream>
#include <string>
#include <vector>
#include "Point.h"
#include <chrono>

class DataWriter
{
public:
	DataWriter(std::string filePath);
	~DataWriter();
	void write(std::vector<Point>* data);
	void writeClusterIds(std::vector<Point>* data);
private:
	std::ofstream file;
};

