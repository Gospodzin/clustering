#include "DataLoader.h"
#include <boost/tokenizer.hpp>
#include <strstream>
#include "logging.h"

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
            point.emplace_back(parseDouble(line.c_str() + start));
            start = end + 1;
        }
}

std::vector<Point>* DataLoader::load() {
    LOG("Loading data...");
    TS();
    std::string dataString((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::istrstream dataStream(dataString.c_str(), dataString.length());
    int pointsCount = std::count(dataString.begin(), dataString.end(), '\n');
    if(dataString.back() != '\n') ++pointsCount;
    std::vector<Point>* data = new std::vector<Point>(pointsCount, Point(dimsCount, -1));
    std::string line;
    while (std::getline(dataStream, line))
        strToPoint(line, data->at(idCarrier));
    TP();

    return data;
}
