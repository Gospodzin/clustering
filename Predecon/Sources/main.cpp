#include "SubcluUI.h"
#include "Dbscan.h"
#include "MTIDataSet.h"
#include "PLDataSet.h"
#include "Qscan.h"
#include <numeric>
#include <chrono>
#include "ODC.h"
#include "VaFileDataSet.h"


int main(int ac, char* av[])
{

	Data* data = DataLoader("hehe.txt").load();

	Qscan qscan(data, 4000, 4);
	qscan.compute();
	system("pause");
	return 0;
}

