#include "DataLoader.h"
#include <boost/tokenizer.hpp>
#include <sstream>
#include <algorithm>
#include <strstream>
#include <iterator>
#include <memory>

DataLoader::DataLoader(std::string filePath) : file(filePath), dimsCount(readDimsCount()), idCarrier(0) {
}


DataLoader::~DataLoader() {
	file.close();
}

Point DataLoader::strToPoint(const std::string& line)
{
	boost::char_separator<char> separators(
		" ", "", boost::drop_empty_tokens); // empty token policy
	boost::tokenizer<boost::char_separator<char> > tok(line, separators);
	Point point(dimsCount, idCarrier++);
	std::for_each(tok.begin(), tok.end(), [&](const std::string& str)->void{point.push_back(std::stod(str)); });
	return point;
}

int DataLoader::readDimsCount()
{
	std::string line;
	std::getline(file, line).seekg(0, std::ios_base::beg);
	return std::count(line.begin(), line.end(), ' ') + 1;
}

std::shared_ptr<BasicDataSet> DataLoader::load() {
	std::string dataString((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	std::istrstream dataStream(dataString.c_str(), dataString.length());
	std::shared_ptr<BasicDataSet> data(new BasicDataSet(std::count(dataString.begin(), dataString.end(), '\n')));
	std::string line;
	while (std::getline(dataStream, line))
		data->push_back(strToPoint(line));
	return data;
}