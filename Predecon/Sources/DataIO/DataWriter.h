#pragma once
#include <fstream>
#include <string>
#include <vector>
#include "Clustering/Point.h"
#include <chrono>
#include <map>
#include "Clustering/Cluster.h"

class DataWriter
{
public:
	DataWriter(std::string filePath);
	~DataWriter();
	void write(std::string str);
	std::string write(std::vector<Point>* data);
    std::string write(std::vector<Point>* data, std::vector<std::vector<int>>& prefDims);
    std::string write(const std::map < std::vector <int>, std::vector<Cluster*> >& clustersBySubspace);
	void writeClusterIds(std::vector<Point>* data);
	std::string write(const std::vector<int>& subspace);
private:
	std::ofstream file;
};

