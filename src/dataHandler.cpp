#include "dataHandler.hpp"

SingleCSVDataHandler::SingleCSVDataHandler(std::queue<std::shared_ptr<Event>>* eventQueue, std::string csvDirectory, std::vector<std::string> symbols, bool* continueBacktest) {
    this->eventQueue = eventQueue;
    this->csvDirectory = csvDirectory;
    this->symbols = symbols;
    this->continueBacktest = continueBacktest;
    this->data;
    this->consumedData;
    this->bar;
    loadData();
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
            lineVector.emplace_back(lineItem);
        }

        innerMap.insert({ std::stoll(lineVector[0]), std::make_tuple(std::stod(lineVector[3]), std::stod(lineVector[4]), std::stod(lineVector[5]), std::stod(lineVector[6]), std::stod(lineVector[8])) });
    }

    this->data.insert(std::make_pair(symbols[0], innerMap));

    // initialize iterator over the data
    this->bar = data[symbols[0]].begin();

    // initialize consumedData[symbol]
    innerMap.clear();
    this->consumedData.insert(std::make_pair(symbols[0], innerMap));
}

std::vector<std::tuple<double, double, double, double, double>> SingleCSVDataHandler::getLatestBars(std::string* symbol, int n) {
    std::vector<std::tuple<double, double, double, double, double>> res;
    res.reserve(n);

    // Returns an empty object if there is not enough data in "consumedData"
    if (this->consumedData[*symbol].size() < n) return res;

    for (auto rit = this->consumedData[*symbol].rbegin(); n > 0 && rit != this->consumedData[*symbol].rend(); ++rit, --n) {
        res.emplace_back(rit->second);
    }

    return res;
} 

void SingleCSVDataHandler::updateBars() {
    // add a bar to "consumedData"
    if (bar != data[symbols[0]].end()) {
        consumedData[symbols[0]][bar->first] =  bar->second;
        ++bar;
    } else {
        *continueBacktest = false;
    }

    eventQueue->push(std::make_shared<MarketEvent>());
}