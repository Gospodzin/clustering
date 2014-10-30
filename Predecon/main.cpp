#include <iostream>
#include <chrono>
#include <boost/tokenizer.hpp>
#include "DataIO.h"
#include "TIDataSet.h"
#include "measures.h"
#include "Predecon.h"


using namespace std;

clock_t beginTick;

static void tS() {
	beginTick = clock();
}

static void tP() {
	printf("%f\n", double(clock() - beginTick) / CLOCKS_PER_SEC);
}

int main()
{
	
		tS();
		vector<Point>* data = DataLoader("small.txt").load();
		tP();
		tS();
		TIDataSet dataSet(data, referenceSelectors::max, measures::euclideanDistance);
		Predecon<TIDataSet> predecon(&dataSet, measures::euclideanDistance, 400.0, 3, 1.8, 999999999);
		tP();
		tS();
		predecon.compute();
		tP();
		tS();
		DataWriter("out.txt").write(data);
		tP();
 return 0;
}

