#include "SubcluUI.h"
#include "AlgorithmTester.h"


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
	Data* data = DataLoader("testData.txt").load();
	TIDataSet dataSet(data, measures::MeasureId::Euclidean, referenceSelectors::max);
	Subclu subclu(&dataSet, 10000, 30);
	subclu.compute();

	//SubcluUI().run(ac, av);
	return 0;
}
