#pragma once
#include <string>
#include <fstream>
#include "Data.h"

class DataLoader
{
public:
	DataLoader(std::string filePath);
	~DataLoader();
	Data* load(bool headers = false);
private:
    double parseDouble(const char* number);
    void strToPoint(std::string& line, Point& point);
	void readHeaders(std::string headers, Data& data);
    int countPoints(std::string& dataString, bool headers);
	int readDimsCount();
	std::ifstream file;
	int dimsCount;
	int idCarrier;
};

