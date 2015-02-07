#include "StatsCollector.h"
#include <fstream>
#include "Logging/logging.h"
#include <sstream>



StatsCollector::StatsCollector()
{
}


StatsCollector::~StatsCollector()
{
}

void StatsCollector::collect(std::vector<Point>& data) {
	LOG("Collecting statistics...");
	TS();
	for (Point& p : data) {
		if (stats.size() <= p.cid)
			stats.resize(p.cid+1);
		++stats[p.cid];
	}
	TP();
}

std::string StatsCollector::toString(double time) {
	LOG("Writing statistics to string...");
	TS();
	std::stringstream ss;
	ss << "No. of clusters found: " << stats.size() - 1 << std::endl;
	ss << "Duration [s]: " << time << std::endl;
	ss << "No. of noise points : " << stats[0] << std::endl;
	ss << "[Cluster id] : [Cluster size]" << std::endl;
	for (size_t i = 1; i < stats.size(); i++) {
		ss << i << " : " << stats[i] << std::endl;
	}

	return ss.str();
	TP();
}

void StatsCollector::write(std::string filePath, std::string statsStr) {
	LOG("Writing statistics to file...");
	TS();
	std::ofstream file(filePath);
	file << statsStr;
	file.close();
	TP();
}