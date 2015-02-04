#include <stack>
#include "logging.h"
#include "BasicDataSet.h"

//template <typename T>
class Subclu
{
public:
	Subclu(BasicDataSet* data, double eps, int mi) : data(data), eps(eps), mi(mi) {}

	BasicDataSet* const data;
	const double eps;
	const int mi;

	void run() {
		subclu();
	}
private:
	void subclu() {
		// STEP 1 Generate all 1-D clusters
		for (int a = 0; a < data->dimensions(); ++a) {

		}
	}
};