#include "TIDataSet.h"
#include <stack>

class Predecon
{
public:
	std::shared_ptr<TIDataSet> dataSetP;
	TIDataSet& dataSet;

	Predecon(std::shared_ptr<TIDataSet> dataSet) : dataSetP(dataSet), dataSet(*dataSet), cid(NOISE){}
	
	void compute() {
		for (int i = 0; i < dataSet.size(); ++i) {
			if (dataSet[i].cid == NONE) {
				std::stack<Point*> seeds;
				//core point
				Point& core = dataSet[i];
				// Twórz grupê na podstawie pierwszego dotychczas niesklasyfikowanego punktu p
				core.cid = nextCid();
				// wstaw punkt p do seeds;
				seeds.push(&core);
				while (!seeds.empty()) {
					Point& point = *seeds.top();
					seeds.pop();
					auto& prefNeighbourhood = dataSet.prefNeighbourhoods[point.id];
					for (Point* p : prefNeighbourhood) {
						if (p->cid == NONE) {
							p->cid = curCid();
							seeds.push(p);
						}
						else if (dataSet.pDims[p->id] <= dataSet.lambda)
							p->cid = curCid();
					}
				}
			}
		}
	}

private:
	inline int& nextCid() {
		return ++cid;
	}
	inline int& curCid() {
		return cid;
	}
	int cid;
};