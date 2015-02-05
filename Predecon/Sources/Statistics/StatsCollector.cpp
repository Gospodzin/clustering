#include "StatsCollector.h"
#include <fstream>
#include "Logging/logging.h"


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

void StatsCollector::write(std::string filePath, double time) {
	LOG("Writing statistics to file...");
	TS();
	std::ofstream file(filePath);
	file << "No. of clusters found: " << stats.size() - 1 << std::endl;
	file << "Duration: " << time << std::endl;
	file << "N : " << stats[0] << std::endl;
	for (int i = 1; i < stats.size(); i++) {
		file << i << " : " << stats[i] << std::endl;
	}
	file.close();
	TP();
}