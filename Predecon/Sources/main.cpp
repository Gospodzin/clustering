#include "SubcluUI.h"
#include "Dbscan.h"
#include "PLDataSet.h"
#include "Qscan.h"
#include <numeric>
#include <chrono>
#include "ODC.h"
#include "VaFileDataSet.h"


int main(int ac, char* av[])
{
	int n;
	std::cin >> n;
	Data data(10, Point(std::vector<double>(n, 1), 1));
	
	utils::pca(data, n);
	system("pause");
	return 0;
}

