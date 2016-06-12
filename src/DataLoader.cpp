#include "DataLoader.h"
#include <boost/tokenizer.hpp>
#include <strstream>
#include "logging.h"
#include "utils.h"

DataLoader::DataLoader(std::string filePath) : file(filePath), dimsCount(readDimsCount()), idCarrier(0) {
}

DataLoader::~DataLoader() {
    file.close();
}

int DataLoader::readDimsCount()
{
    std::string line;
    std::getline(file, line).seekg(0, std::ios_base::beg);
    int dimsCount = std::count(line.begin(), line.end(), ' ') + 1;

    return dimsCount;
}

// more efficient parsing than stod
double DataLoader::parseDouble(const char* number) {
    bool sign = false;
    double prePoint = 0;
    if(*number == '-') sign = true, ++number;
    while(*number != '\0' && *number !='.')  {
        prePoint = prePoint*10 + *number - '0';
        ++number;
    }
    if(*number == '.') ++number;
    double postPoint = 0;
    double tenPow = 1;
    while(*number != '\0')  {
        tenPow*=10;
        postPoint = postPoint*10 + *number - '0';
        ++number;
    }

    return (sign ? -1 : 1) * (prePoint + postPoint / tenPow);
}

void DataLoader::strToPoint(std::string& line, Point& point) {
    point.id = idCarrier++;
    unsigned l = line.length() + 1;
    // more efficient tokenization than boost tokenize
    for(unsigned start = 0, end = 0; end < l; ++end)
        if(line[end] == ' ' || line[end] == '\0') {
            line[end] = '\0';
            //point.emplace_back(parseDouble(line.c_str() + start));
			point.emplace_back(std::atof(line.c_str() + start)); // supports scientific notation
            start = end + 1;
        }
}

void DataLoader::readHeaders(std::string headers, Data& data) {
	boost::tokenizer<boost::char_separator<char>> tokens(headers, boost::char_separator<char>(" "));
	for(const auto& header : tokens) data.headers.emplace_back(header);
}

int DataLoader::countPoints(std::string& dataString, bool headers) {
    int pointsCount = std::count(dataString.begin(), dataString.end(), '\n');
    if(dataString.back() != '\n') ++pointsCount;
    if(headers) --pointsCount;

    return pointsCount;
}

Data* DataLoader::load(bool headers) {
    TS("Loading data...");
    std::string dataString((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::istrstream dataStream(dataString.c_str(), dataString.length());
    int pointsCount = countPoints(dataString, headers);
    Data* data = new Data(pointsCount, Point(dimsCount, -1));
    std::string line;
	if(headers && std::getline(dataStream, line)) readHeaders(line, *data);
    while (std::getline(dataStream, line)) strToPoint(line, data->at(idCarrier));
    if(!headers) utils::defaultHeaders(*data);
    TP("Data loaded");

    return data;
}
