#pragma once
#include <iostream>
#include <string>
#include "dataio.h"
#include "clustering.h"
#include "StatsCollector.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iterator>
using namespace std;

enum DataStructure {
	TI,
	Basic
};

struct Settings {
	bool help = false;
	std::string path;
	double eps = -1;
	int mi = -1;
	double delta = -1;
	int lambda = -1;
	double kappa = -1;
	int pt = 7;
	double ps = 0.2;
	measures::Measures measure = static_cast<measures::Measures>(-1);
	DataStructure dataStructure = static_cast<DataStructure>(-1);
};

class UI {
public:
	void run(int ac, char* av[]) {
		Settings sets = processProgramOptions(ac, av);
		if (sets.help) return;
		readSettings(sets);
		std::vector<Point>* data = DataLoader(sets.path).load();
		std::string fileName = getFileNameFromPath(sets.path);
		clock_t t = clock();
		switch (sets.dataStructure){
			case TI: {
				TIDataSet dataSet(data, measures::getMeasure(sets.measure), referenceSelectors::max);
				performPredecon(dataSet, sets);
				break;
			}
			case Basic: {
				BasicDataSet dataSet(data, measures::getMeasure(sets.measure));
				performPredecon(dataSet, sets);
				break;
			}
		}
		double time = double((clock() - t)) / CLOCKS_PER_SEC;
		char outPath[1000];
		sprintf_s(outPath, "%s_%f_%d_%f_%d.txt", fileName.c_str(), sets.eps, sets.mi, sets.delta, sets.lambda);
		DataWriter("out_" + std::string(outPath)).writeClusterIds(data);
		StatsCollector sc = stats(data, outPath, time);
		drawData(data, outPath, sets, sc);
	}

private:
	Settings processProgramOptions(int ac, char* av[]) {
		Settings sets;
		try {
			po::options_description desc("Allowed options");
			desc.add_options()
				("help", "produce help message")
				("path", po::value<std::string>(), "set path")
				("eps", po::value<double>(), "set eps")
				("mi", po::value<int>(), "set mi")
				("delta", po::value<double>(), "set delta")
				("lambda", po::value<int>(), "set lambda")
				("kappa", po::value<double>(), "set kappa")
				("measure", po::value<int>(), "set measure [Euclidean -> 0, Manhattan -> 1]")
				("ds", po::value<int>(), "set data structure [TI -> 0, Basic -> 1]")
				("style.pt", po::value<int>(), "point type")
				("style.ps", po::value<double>(), "point size")
				;

			po::variables_map vm;
			po::store(po::parse_command_line(ac, av, desc), vm);
			po::store(po::parse_config_file<char>("settings.ini", desc), vm);
			po::notify(vm);

			if (vm.count("help")) {
				cout << desc << "\n";
				sets.help = true;
			}

			if (vm.count("path")) sets.path = vm["path"].as<std::string>();
			if (vm.count("eps")) sets.eps = vm["eps"].as<double>();
			if (vm.count("mi")) sets.mi = vm["mi"].as<int>();
			if (vm.count("delta")) sets.delta= vm["delta"].as<double>();
			if (vm.count("lambda")) sets.lambda= vm["lambda"].as<int>();
			if (vm.count("kappa")) sets.kappa = vm["kappa"].as<double>();
			if (vm.count("measure")) sets.measure = static_cast<measures::Measures>(vm["measure"].as<int>());
			if (vm.count("ds")) sets.dataStructure = static_cast<DataStructure>(vm["ds"].as<int>());
			if (vm.count("style.pt")) sets.pt= vm["style.pt"].as<int>();
			if (vm.count("style.ps")) sets.ps = vm["style.ps"].as<double>();

		}
		catch (exception& e) {
			cerr << "error: " << e.what() << "\n";
		}
		catch (...) {
			cerr << "Exception of unknown type!\n";
		}
		return sets;
	}

	std::string getFileNameFromPath(std::string path) {
		return path.substr(path.find_last_of("\\") + 1);
	}

	void readSettings(Settings& sets) {
		int tmpEnumId;
		if (sets.path.empty()) {
			std::cout << "Enter data file path: ";
			std::cin >> sets.path;
		}
		if (sets.measure == -1) {
			std::cout << "Choose measure: \n" << measures::Euclidean << ". Euclidean\n" << measures::Manhattan << ". Manhattan\n";
			std::cin >> tmpEnumId;
			sets.measure = static_cast<measures::Measures>(tmpEnumId);
		}
		if (sets.dataStructure == -1) {
			std::cout << "Choose data structure: \n" << TI << ". TIPredecon\n" << Basic << ". BasicPredecon\n";
			std::cin >> tmpEnumId;
			sets.dataStructure = static_cast<DataStructure>(tmpEnumId);
		}
		if (sets.eps == -1) {
			std::cout << "eps: ";
			std::cin >> sets.eps;
		}
		if (sets.mi == -1) {
			std::cout << "mi: ";
			std::cin >> sets.mi;
		}
		if (sets.delta == -1) {
			std::cout << "delta: ";
			std::cin >> sets.delta;
		}
		if (sets.lambda == -1) {
			std::cout << "lambda: ";
			std::cin >> sets.lambda;
		}
		if (sets.kappa == -1) {
			std::cout << "kappa: ";
			std::cin >> sets.kappa;
		}
	}

	template<typename T>
	void performPredecon(T& dataSet, Settings sets) {
		Predecon<T> predecon(&dataSet, measures::getPrefMeasure(sets.measure), sets.eps, sets.mi, sets.delta, sets.lambda, sets.kappa);
		predecon.compute();
	}

	StatsCollector stats(std::vector<Point>* data, std::string outPath, double time) {
		StatsCollector sc;
		sc.collect(*data);
		sc.write("stt_" + std::string(outPath), time);
		return sc;
	}

	void drawData(std::vector<Point>* data, std::string output, Settings sets, StatsCollector sc) {
		DataWriter("grh_" + output).write(data);
		std::stringstream title;
		title << "e:" << sets.eps << " m:" << sets.mi << " l:" << sets.lambda << " d:" << sets.delta << " n:" << sc.stats.size() - 1;
		std::stringstream ss;
		ss << "gnuplot -e \"set view equal xy;plot 'grh_" << output << "' u 2:3:1 title '" + title.str() + "' with points palette pt " << sets.pt << " ps " << sets.ps << ";set view equal xy;\" -p";
		system(ss.str().c_str());
	}
};

