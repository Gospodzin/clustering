#include <iostream>
#include <chrono>
#include <boost/tokenizer.hpp>
#include "dataio.h"
#include "TIDataSet.h"
#include "measures.h"
#include "Predecon.h"
#include "logging.h"
#include "UI.h"
#include "Dbscan.h"
#include "Subclu.h"

using namespace std;

int main(int ac, char* av[])
{
	UI().run(ac, av);
	return 0;
}
