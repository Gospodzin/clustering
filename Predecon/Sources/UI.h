#pragma once
#include <iostream>
#include <string>
#include <iterator>

#include "dataio.h"
#include "clustering.h"
#include "StatsCollector.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;


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
	measures::MeasureId measureId = static_cast<measures::MeasureId>(-1);
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
		std::vector<std::vector<int> > prefDims;
		switch (sets.dataStructure){
			case TI: {
				TIDataSet dataSet(data, sets.measureId, referenceSelectors::max);
				prefDims = performPredecon(dataSet, sets);
				break;
			}
			case Basic: {
				BasicDataSet dataSet(data, sets.measureId);
				prefDims = performPredecon(dataSet, sets);
				break;
			}
		}
		double time = double((clock() - t)) / CLOCKS_PER_SEC;
		std::stringstream ss;
		ss << "e=" << sets.eps << "_m=" << sets.mi << "_d=" << sets.delta << "_l=" << sets.lambda;
		DataWriter dw("out_" + std::string(ss.str())+".txt");
		dw.write(dw.write(data, prefDims));
		StatsCollector sc = stats(data, ss.str(), time);
		drawData(data, ss.str(), sets, sc);
	}

private:
	Settings processProgramOptions(int ac, char* av[]) {
		Settings sets;
		try {
			po::options_description desc("Allowed options");
			desc.add_options()
				("help", "produce help message")
				("general.path", po::value<std::string>(), "set path")
				("clustering.eps", po::value<double>(), "set eps")
				("clustering.mi", po::value<int>(), "set mi")
				("clustering.measure", po::value<int>(), "set measure [Euclidean -> 0, Manhattan -> 1]")
				("clustering.ds", po::value<int>(), "set data structure [TI -> 0, Basic -> 1]")
				("predecon.delta", po::value<double>(), "set delta")
				("predecon.lambda", po::value<int>(), "set lambda")
				("predecon.kappa", po::value<double>(), "set kappa")
				("style.pt", po::value<int>(), "point type")
				("style.ps", po::value<double>(), "point size")
				;

			po::variables_map vm;
			po::store(po::parse_command_line(ac, av, desc), vm);
			po::store(po::parse_config_file<char>("settings.ini", desc), vm);
			po::notify(vm);

			if (vm.count("help")) {
				std::cout << desc << "\n";
				sets.help = true;
			}

			if (vm.count("general.path")) sets.path = vm["general.path"].as<std::string>();
			if (vm.count("clustering.eps")) sets.eps = vm["clustering.eps"].as<double>();
			if (vm.count("clustering.mi")) sets.mi = vm["clustering.mi"].as<int>();
			if (vm.count("clustering.measure")) sets.measureId = static_cast<measures::MeasureId>(vm["clustering.measure"].as<int>());
			if (vm.count("clustering.ds")) sets.dataStructure = static_cast<DataStructure>(vm["clustering.ds"].as<int>());
			if (vm.count("predecon.delta")) sets.delta= vm["predecon.delta"].as<double>();
			if (vm.count("predecon.lambda")) sets.lambda= vm["predecon.lambda"].as<int>();
			if (vm.count("predecon.kappa")) sets.kappa = vm["predecon.kappa"].as<double>();
			if (vm.count("style.pt")) sets.pt= vm["style.pt"].as<int>();
			if (vm.count("style.ps")) sets.ps = vm["style.ps"].as<double>();

		}
		catch (std::exception& e) {
			std::cerr << "error: " << e.what() << "\n";
		}
		catch (...) {
			std::cerr << "Exception of unknown type!\n";
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
		if (sets.measureId == -1) {
			std::cout << "Choose measure: \n" << measures::Euclidean << ". Euclidean\n" << measures::Manhattan << ". Manhattan\n";
			std::cin >> tmpEnumId;
			sets.measureId = static_cast<measures::MeasureId>(tmpEnumId);
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
	std::vector<std::vector<int> > performPredecon(T& dataSet, Settings sets) {
		Predecon<T> predecon(&dataSet, sets.eps, sets.mi, sets.delta, sets.lambda, sets.kappa);
		predecon.compute();
		return predecon.prefDims;
	}

	StatsCollector stats(std::vector<Point>* data, std::string outPath, double time) {
		StatsCollector sc;
		sc.collect(*data);
		sc.write("stt_" + std::string(outPath) + ".txt", sc.toString(time));
		return sc;
	}

	void drawData(std::vector<Point>* data, std::string output, Settings sets, StatsCollector sc) {
		DataWriter("grh_" + output + ".txt").writeGraph(data);
		std::stringstream title;
		title << "e:" << sets.eps << " m:" << sets.mi << " l:" << sets.lambda << " d:" << sets.delta << " n:" << sc.stats.size() - 1;
		std::stringstream ss;
		ss << "gnuplot\\bin\\gnuplot.exe -e \"set view equal xy;plot 'grh_" << output << ".txt' u 2:3:1 title '" + title.str() + "' with points palette pt " << sets.pt << " ps " << sets.ps << ";set view equal xy;\" -p";
		DataWriter("sgr_" + output + ".bat").write(ss.str());
		system(ss.str().c_str());
	}
};

