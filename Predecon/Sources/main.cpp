#include "SubcluUI.h"
#include "PLDataSet.h"
#include "Qscan.h"
#include <numeric>
#include <chrono>
#include "OneDClustering.h"
#include "ODC.h"
#include "RTreeDataSet.h"

std::vector<Point> extractData(std::vector<Point>& data, std::vector<int> attrs) {
	std::vector<Point> extractedData;
	extractedData.reserve(data.size());
	for (Point& p : data) {
		extractedData.emplace_back(attrs.size(), p.id);
		for (int a : attrs)
			extractedData.back().push_back(p[a]);
	}

	return extractedData;
}

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
	//
	//int attr = 0;
	//double eps = 100;
	//int mi = 10;
	//std::string path = "dense_d55_r581012_covtype.txt";

	//Data* data0 = DataLoader(path).load();
	//TIDataSet dataSet(data0, measures::MeasureId::Euclidean, referenceSelectors::max);
	//Dbscan<TIDataSet> dbscan(&dataSet, eps, mi, { attr });
	//dbscan.compute();

	//Data* data1 = DataLoader(path).load();
	//OneDClustering odc1(data1, eps, mi, attr);
	//odc1.compute();
	////std::cout << DataWriter::write(data1)<<std::endl;
	//Data* data2 = DataLoader(path).load();
	//ODC odc2(data2, eps, mi, attr);
	//odc2.compute();
	////std::cout << DataWriter::write(data2) << std::endl;

	//int errs=0;
	//for (int i = 0; i < data1->size(); ++i)
	//	errs += data1->at(i).cid != data2->at(i).cid || data1->at(i) != data0->at(i);

	//std::cout << errs << std::endl;

	//Data* data = DataLoader("dense_d2_r62556_sequoia.txt").load();
	//TS()
	//	std::vector<Point> dc = extractData(*data, { 0 });
	//TP()
	//	std::cout << dc[44][0] << std::endl;
	//Subclu<TIDataSet> subclu(data, 10000, 30);
	//subclu.compute();

	Data* data = DataLoader("dense_d56_r96367_cup98_quarter.txt").load();
	Data* d = utils::pca(*data, 2);

	TS();
	//utils::maxVarDir(*data);
	TP();
	/*auto max = referenceSelectors::max(*data);
	auto min = referenceSelectors::min(*data);

	std::stringstream ss;
	for(Point& p : *data) {
		bool first = true;
		for(int i = 0; i < data->dimensions(); ++i)
			if(max[i] != min[i]) {
				ss << (first ? "" : " ") << p[i];
				first = false;
			}
		ss << std::endl;
	}
	DataWriter("out.txt").write(ss.str());*/
	/*std::vector<Point> sample = utils::randomSample(*data, 10000);

	auto means = utils::calcMeans(*data);
	std::stringstream ss;
	ss << "param n:="<<sample.size()<<";\r\n"
		"param d:="<<data->dimensions()<<";\r\n"
		"param r : 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 :=\r\n";
	int id = 1;
	for(auto& p : sample) {
		ss << id++;
		for(int i = 0; i < data->dimensions(); ++i)
			ss << " " << p[i] - means[i];
		ss << std::endl;
	}
	ss << ";";
	DataWriter("out.txt").write(ss.str());*/
	system("pause");
	//tests::runTests();
	return 0;
}

