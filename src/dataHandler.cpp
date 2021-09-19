#include "dataHandler.hpp"

SingleCSVDataHandler::SingleCSVDataHandler(std::queue<Event>* eventQueue, std::string csvDirectory, std::string symbol, bool* continueBacktest) {
    this->eventQueue = eventQueue;
    this->csvDirectory = csvDirectory;
    this->symbol = symbol;
    this->continueBacktest = continueBacktest;
};

void SingleCSVDataHandler::loadData() {
    std::ifstream fileToRead(csvDirectory, std::ios::binary);
    if (!fileToRead.is_open()) throw std::runtime_error("Could not open file!");

    std::string line, lineItem;
    std::map<long long, std::tuple<double, double, double, double, double>> innerMap;

    // skip the first two rows in the data file to get to the raw data
    std::getline(fileToRead, line);
    std::getline(fileToRead, line);

    // read the raw data line by line
    while (std::getline(fileToRead, line)) {
        std::stringstream ss(line);
        std::vector<std::string> lineVector;

        while (std::getline(ss, lineItem, ',')) {
            lineVector.push_back(lineItem);
        }
        
        innerMap.insert(std::make_pair(std::stoll(lineVector[0]), std::make_tuple(std::stod(lineVector[3]), std::stod(lineVector[4]), std::stod(lineVector[5]), std::stod(lineVector[6]), std::stod(lineVector[8]))));
    }

    data.insert(std::make_pair(symbol, innerMap));

    // initialize iterator over the data
    bar = data.at(symbol).begin();
}

std::unordered_map<std::string, std::map<long long, std::tuple<double, double, double, double, double>>> SingleCSVDataHandler::getLatestBars(std::string symbol, int n = 1) {
    std::unordered_map<std::string, std::map<long long, std::tuple<double, double, double, double, double>>> lastNBars;
    std::map<long long, std::tuple<double, double, double, double, double>> innerMap;

    auto map = consumedData.at(symbol);
    for (auto rit = map.crbegin(); n > 0 && rit != map.crend(); ++rit, --n) {
        innerMap.insert(std::make_pair(rit->first, rit->second));
    }

    lastNBars.insert(std::make_pair(symbol, innerMap));

    return lastNBars;
} 

void SingleCSVDataHandler::updateBars() {
    // add a bar to "consumedData"
    if (bar != data.at(symbol).end()) {
        consumedData.at(symbol).at(bar->first) = bar->second;
        ++bar;
    } else {
        *continueBacktest = false;
    }

    eventQueue->push(MarketEvent());
}