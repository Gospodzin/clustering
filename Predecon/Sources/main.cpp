#include "SubcluUI.h"
#include "Dbscan.h"
#include "MTIDataSet.h"
#include "PLDataSet.h"
#include "Qscan.h"
#include <numeric>
#include <chrono>
#include "ODC.h"
#include "VaFileDataSet.h"
#include "Subclu.h"

Data genData(int n, int d) {
	Data data;
	data.reserve(n);
	for (int i = 0; i < n; ++i) {
		Point p(d, i);
		for (int j = 0; j < d; ++j)
			p.push_back(rand()%10000);
		data.push_back(p);
	}

	return data;
}

int main(int ac, char* av[])
{
	srand(time(NULL));

	//std::vector<long> clocks;
	//std::vector<long> clocks2;
	//std::vector<int> is;

	//for (int i = 2500; i <= 200000; i += 2500)
	//{
	//	is.push_back(i);
	//	//double eps = 200.0;
	//	//int mi = 5;

	//	Data data2 = genData(i, 1);
	//	//Data data2 = data;

	//	long s = std::clock();
	//	//ODC odc(&data, eps, mi, 0);
	//	//odc.compute();
	//	//clocks.push_back(std::clock() - s);

	//	s = std::clock();
	//	TIDataSet dataSet(&data2, {measures::Euclidean, referenceSelectors::max});
	//	//Dbscan<TIDataSet> dbscan(&dataSet, { eps, mi });
	//	//dbscan.compute();
	//	clocks2.push_back(std::clock() - s);

	//	//int max = 0;
	//	//int max2 = 0;
	//	//for (int i = 0; i < data.size(); ++i) {
	//	//	max = data[i].cid > max ? data[i].cid : max;
	//	//	max2 = data2[i].cid > max2 ? data2[i].cid : max2;

	//	//}
	//	//assert(max == max2);
	//}
	//for (int i: is) std::cout << i << " ";
	//std::cout << std::endl;

	////for (long clock : clocks) std::cout << double(clock) / CLOCKS_PER_SEC << " ";
	////std::cout << std::endl;
	//for (long clock : clocks2) std::cout << double(clock) / CLOCKS_PER_SEC << " ";
	//std::cout << std::endl;


	/*std::vector<double> rateTime;
	std::vector<double> rateCount;
	std::vector<int> ns;
	double eps = 200.0;
	int mi = 10;
	for (int n = 1; n <= 10; ++n) {
		ns.push_back(n);
		Data data = genData(5000, n);
		Subclu<TIDataSet> subclu(&data, { eps, mi, { measures::Euclidean, referenceSelectors::max }, true });
		subclu.compute();
		rateTime.push_back(subclu.time1D / double(subclu.timeNGt1D + subclu.time1D));
		rateCount.push_back(subclu.count1D / double(subclu.countNGt1D + subclu.count1D));
	}
	std::cout << "eps: " << eps << " " << "mi: " << mi << std::endl;
	for (auto v : ns) std::cout << v << " "; std::cout << std::endl;
	for (auto v : rateTime) std::cout << v << " "; std::cout << std::endl;
	for (auto v : rateCount) std::cout << v << " "; std::cout << std::endl;*/

	/*logging::Logger::maxDepth = -1;
	std::vector<double> rateTime;
	std::vector<double> rateCount;
	std::vector<double> epss;
	std::vector<std::vector<int>> clusterCountsByEps;
	std::vector<std::vector<int>> subspaceSizesByEps;
	int n = 5;
	int mi = 10;
	for (double eps = 20; eps <= 2000; eps+=20) {
		epss.push_back(eps);
		Data data = genData(5000, n);
		Subclu<TIDataSet> subclu(&data, { eps, mi, { measures::Euclidean, referenceSelectors::max } });
		subclu.compute();
		rateTime.push_back(subclu.time1D / double(subclu.timeNGt1D + subclu.time1D));
		rateCount.push_back(subclu.count1D / double(subclu.countNGt1D + subclu.count1D));
		auto clustersBySubspace = subclu.getClusters();
		std::vector<int> clusterCounts(n);
		std::vector<int> subspaceSizes(n);
		for (auto clusters : clustersBySubspace) {
			int dim = clusters.first.size();
			int size = clusters.second.size();
			clusterCounts[dim - 1] += size;
			for (auto cluster : clusters.second) {
				subspaceSizes[dim - 1] += cluster.second->points.size();
				delete cluster.second;
			}
		}
		clusterCountsByEps.emplace_back(clusterCounts);
		subspaceSizesByEps.emplace_back(subspaceSizes);

	}
	std::cout << "n: " << n << " " << "mi: " << mi << std::endl;
	for (auto v : epss) std::cout << v << " "; std::cout << std::endl;
	for (auto v : rateTime) std::cout << v << " "; std::cout << std::endl;
	for (auto v : rateCount) std::cout << v << " "; std::cout << std::endl;
	std::cout << std::endl;
	for (auto clusterCounts : clusterCountsByEps) {
		for (auto v : clusterCounts) std::cout << v << " "; std::cout << std::endl;
	}
	std::cout << std::endl;
	for (auto subspaceSizes : subspaceSizesByEps) {
		for (auto v : subspaceSizes) std::cout << v << " "; std::cout << std::endl;
	}
*/
	/*logging::Logger::maxDepth = -1;
	std::vector<double> rateTime;
	std::vector<double> rateCount;
	std::vector<double> mis;
	int n = 5;
	double eps = 1500.0;
	std::vector<std::vector<int>> clusterCountsByEps;
	std::vector<std::vector<int>> subspaceSizesByEps;
	for (int mi = 5; mi <= 505; mi += 10) {
		mis.push_back(mi);
		Data data = genData(5000, n);
		Subclu<TIDataSet> subclu(&data, { eps, mi, { measures::Euclidean, referenceSelectors::max } });
		subclu.compute();
		rateTime.push_back(subclu.time1D / double(subclu.timeNGt1D + subclu.time1D));
		rateCount.push_back(subclu.count1D / double(subclu.countNGt1D + subclu.count1D));

		auto clustersBySubspace = subclu.getClusters();
		std::vector<int> clusterCounts(n);
		std::vector<int> subspaceSizes(n);
		for (auto clusters : clustersBySubspace) {
			int dim = clusters.first.size();
			int size = clusters.second.size();
			clusterCounts[dim - 1] += size;
			for (auto cluster : clusters.second) {
				subspaceSizes[dim - 1] += cluster.second->points.size();
				delete cluster.second;
			}
		}
		clusterCountsByEps.emplace_back(clusterCounts);
		subspaceSizesByEps.emplace_back(subspaceSizes);

	}
	std::cout << "n: " << n << " " << "eps: " << eps << std::endl;
	for (auto v : mis) std::cout << v << " "; std::cout << std::endl;
	for (auto v : rateTime) std::cout << v << " "; std::cout << std::endl;
	for (auto v : rateCount) std::cout << v << " "; std::cout << std::endl;
	std::cout << std::endl;
	for (auto clusterCounts : clusterCountsByEps) {
		for (auto v : clusterCounts) std::cout << v << " "; std::cout << std::endl;
	}
	std::cout << std::endl;
	for (auto subspaceSizes : subspaceSizesByEps) {
		for (auto v : subspaceSizes) std::cout << v << " "; std::cout << std::endl;
	}*/
	//double eps = 0.1;
	//int mi = 10;
	//int N = 10000;
	//int n = 56;

	//Data* rawData = DataLoader("dense_d56_r96367_cup98.txt").load();
	//std::vector<Point> data = utils::randomSample(*rawData, N, 123);
	//Data* data2 = utils::pca(data, 20);
	//DataWriter("dense_d20_r10000_cup98.out").writeData(*data2);
	//for (int i = 0; i < data.size(); ++i) data[i].id = i;
	//utils::normalize(data);



	//Point ref(n, 0);
	//ref.resize(n);
	//ref[37] = 1;	ref[36] = 1;
	//ref[37] = 1;	ref[25] = 1;

	////for (int i = 0; i < ref.size(); ++i) ref[i] = 1;

	//long s = std::clock();

	//TIDataSet tiDataSet(&data, DataSet::Params(measures::Euclidean, ref));
	//Dbscan<TIDataSet> dbscan(&tiDataSet, Algorithm::Params(eps, mi));
	//dbscan.compute();

	//std::cout << double(std::clock() - s) / CLOCKS_PER_SEC << std::endl;

	//int id = -1;
	//Point p[] = { 
	//	Point({ 0, 0 }, ++id),
	//	Point({ 1, 0 }, ++id),
	//	Point({ 2, 0 }, ++id),
	//	Point({ 3, 0 }, ++id),
	//	Point({ 4, 0 }, ++id)};
	//std::vector<Point> data;
	//for (Point& pp : p) data.emplace_back(pp);

	//Qscan qscan(&data, 4000, 4, 2, 1);
	//qscan.compute();
	//auto cls = qscan.getClusters();
	
//int median = 1710;
//int n = 9;
//std::string path = "dense_d55_r581012_covtype";
//Data* data = DataLoader(path+".txt").load();
//Data half1;
//Data half2;
//for (auto& p : *data) {
//	if (p[n] <= median)
//		half1.emplace_back(p);
//	else
//		half2.emplace_back(p);
//}
//
//DataWriter(path + "_half1.txt").writeData(half1);
//DataWriter(path + "_half2.txt").writeData(half2);


	//Data* data = DataLoader("dense_d55_r581012_covtype.txt").load();
	//Data* data2 = utils::randomSampleNoDim(*data, 50000);
	//PLDataSet dataSet(data2, { measures::Euclidean, std::vector<int>({ 0 }) });
	//Dbscan<PLDataSet> dbscan(&dataSet, { 120, 4 });
	//dbscan.compute();

	Data* data = DataLoader("dense_d55_r581012_covtype.txt").load();
	Data* data2 = utils::randomSampleNoDim(*data, 50000);

	TIDataSet dataSet(data2, { measures::Euclidean, referenceSelectors::getMinMax(1000, measures::Euclidean) });
	Dbscan<TIDataSet> dbscan(&dataSet, { 120, 4 });
	dbscan.compute();


	system("pause");
	return 0;
}

