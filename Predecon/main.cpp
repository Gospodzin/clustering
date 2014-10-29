#include <iostream>
#include <chrono>
#include <boost/tokenizer.hpp>
#include "DataLoader.h"


using namespace std;

int main()
{
	clock_t begin = clock();

	vector<Point>* data = DataLoader("dense_d55_r581012_covtype.txt").load();
	
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout << elapsed_secs << endl;;
	delete data;

	system("pause");
	return 0;
}

