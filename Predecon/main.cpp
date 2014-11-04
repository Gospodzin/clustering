#include <iostream>
#include <chrono>
#include <boost/tokenizer.hpp>
#include "dataio.h"
#include "TIDataSet.h"
#include "measures.h"
#include "Predecon.h"
#include "logging.h"
#include "UI.h"

using namespace std;


int main()
{
	//UI().run();
	vector<Point>* data = DataLoader("a2.txt").load();
	TIDataSet dataSet(data, measures::euclideanDistanceSquared, referenceSelectors::max);
	Predecon<TIDataSet> predecon(&dataSet, measures::euclideanDistanceSquared, 400.0*400.0, 3, 1.8, 999999999);
	predecon.compute();
	DataWriter("out.txt").write(data);
	return 0;
}

