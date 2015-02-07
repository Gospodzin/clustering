#include "UI/UI.h"
#include "Statistics\AlgorithmTester.h"


int main(int ac, char* av[])
{
	system("pause");
	Data* data = DataLoader("dense_d2_r62556_sequoia.txt").load();
	AlgorithmTester at(*data);
	int n = 1000;
	double eps = 10000;
	int mi = 30;
	double delta = 0;
	int lambda = 99;
	at.writeTestData("out.txt", at.testPredecon(TestVar::N, TestDs::TiDS, 1000, 60000, 1000, n, eps, mi, delta, lambda, measures::Euclidean));
	//UI().run(ac, av);
	return 0;
}
