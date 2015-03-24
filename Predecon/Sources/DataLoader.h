#pragma once
#include <string>
#include <fstream>
#include "Point.h"

class DataLoader
{
public:
	DataLoader(std::string filePath);
	~DataLoader();
	std::vector<Point>* load();
private:
    double parseDouble(const char* number);
    void strToPoint(std::string& line, Point& point);
	int readDimsCount();
	std::ifstream file;
	int dimsCount;
	int idCarrier;
};

