#include "UI/UI.h"
#include "Statistics\AlgorithmTester.h"

int main(int ac, char* av[])
{
	Data* data = DataLoader("test2.txt").load();
	AlgorithmTester at(*data);
//	at.test(10, data->size(), 2);
	UI().run(ac, av);
	return 0;
}
