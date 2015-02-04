#pragma once
#include <fstream>
#include <string>
#include <vector>
#include "Point.h"
#include <chrono>
#include <map>
#include "Cluster.h"

class DataWriter
{
public:
	DataWriter(std::string filePath);
	~DataWriter();
	void write(std::vector<Point>* data);
	void writeClusterIds(std::vector<Point>* data); 
	void writeClusters(std::vector < std::map < std::vector <int>, std::vector<Cluster*> > > clustersByDim);
private:
	std::ofstream file;
};

