#pragma once
#include <vector>
#include <sstream>

const int NONE = -1;
const int NOISE = 0;

struct Point : std::vector<double>
{
public:
	Point(int dimsCount, int id) : id(id), cid(NONE) { 
		reserve(dimsCount);
	}

	int id;
	int cid;

	std::string toString() {
		std::stringstream ss;
		ss << "[" << cid << "]";
		for (double& v : *this)
			ss << " " << v;
		return ss.str();
	}
};

