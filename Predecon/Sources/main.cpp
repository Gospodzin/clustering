#include "SubcluUI.h"
#include "AlgorithmTester.h"
#include "ClusteringTests.h"
#include <numeric>
#include <chrono>


int main(int ac, char* av[])
{
	/*Data* data = DataLoader("dense_d2_r62556_sequoia.txt").load();
	AlgorithmTester at(*data);
	TestParams tp;
	tp.testVar = N;
	tp.testDs = BasicDS;
	tp.measureId = measures::MeasureId::Euclidean;
	tp.from = 1000;
	tp.step = 1000;
	tp.to = 20001;
	tp.n = 25000;
	tp.eps = 10000;
	tp.mi = 30;
	tp.adjustMi = false;
	tp.delta = 0;
	tp.lambda = 99;
	std::stringstream ss;
	ss << "out_P" << tp.str() << ".txt";
	at.writeTestData(ss.str(), at.testPredecon(tp));*/
	
	Data* data = DataLoader("dense_d2_r62556_sequoia.txt").load();
	Subclu<TIDataSet> subclu(data, 10000, 30);
	subclu.compute();
	//system("pause");
	//tests::runTests();
	return 0;
}
