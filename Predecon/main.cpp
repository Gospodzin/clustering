#include <iostream>
#include <functional>
#include <chrono>
#include <boost/tokenizer.hpp>
#include "Point.h"
#include "DataLoader.h"
#include <sstream>
#include <strstream>
#include "TIDataSet.h"


using namespace std;

int main()
{
	clock_t begin = clock();

	//shared_ptr<BasicDataSet> data = DataLoader("test_points.csv").load();

	BasicDataSet d(2);
	
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout << elapsed_secs << endl;;


	system("pause");
	return 0;
}

