#include <stack>
#include "logging.h"
#include "BasicDataSet.h"

//template <typename T>
class Dbscan
{
public:
	Dbscan(BasicDataSet* data, double eps, int mi, std::vector<int> attrs) : data(data), eps(eps), mi(mi), attrs(attrs) {}

	BasicDataSet* const data;
	const double eps;
	const int mi;
	std::vector<int> attrs;

	void run() {
		dbscan();
	}

private:
	int clusterId = NOISE + 1;
	std::vector<Point*> seeds;

	void dbscan() {
		for (int i = 0; i < data->size(); ++i) {
			Point* point = &(*data)[i];
			if (point->cid == NONE) 
				if (expandCluster(point)) 
					++clusterId;
		}
	}

	bool expandCluster(Point* point) {
		std::vector<Point*> ngb = data->regionQuery(*point, eps, attrs);
		if (ngb.size() < mi) { // not a core point
			point->cid = NOISE;
			return false; // cluster not found
		}
		else { // core point
			point->cid = clusterId;
			for each (Point* neighbour in ngb) {
				neighbour->cid = clusterId;
				seeds.push_back(neighbour);
			}

			while (!seeds.empty()) { // expand cluster as far as possible
				Point* seed = seeds.back();
				seeds.pop_back();
				std::vector<Point*> seedNgb = data->regionQuery(*seed, eps, attrs);
				if(seedNgb.size() >= mi) // core point
					for each(Point* neighbour in seedNgb)
						if (neighbour->cid == NONE || neighbour->cid == NOISE) {
							neighbour->cid = clusterId;	
							seeds.push_back(neighbour);
						}
			}
			
			return true; // cluster found
		}
	}
};