#include "dataHandler.hpp"

SingleCSVDataHandler::SingleCSVDataHandler(std::queue<Event>* eventQueue, std::string csvDirectory, std::string symbol, bool* continueBacktest) {
    this->eventQueue = eventQueue;
    this->csvDirectory = csvDirectory;
    this->symbol = symbol;
    this->continueBacktest = continueBacktest;
};

void SingleCSVDataHandler::loadData() {
    std::ifstream fileToRead(csvDirectory);
    if (!fileToRead.is_open()) throw std::runtime_error("Could not open file!");

    std::string line, lineItem;
    std::unordered_set<int> validPositions ({0, 2, 3, 4, 5, 6, 8});
    std::unordered_map<long, std::tuple<double, double, double, double, double>> innerMap;

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
        
        innerMap.insert(std::make_pair(std::stol(lineVector[0]), std::make_tuple(std::stod(lineVector[3]), std::stod(lineVector[4]), std::stod(lineVector[5]), std::stod(lineVector[6]), std::stod(lineVector[8]))));
    }

    data.insert(std::make_pair(symbol, innerMap));
}

/*

std::unordered_map<std::string, std::unordered_map<long, std::tuple<double, double, double, double, double>>> getLatestBars(std::string symbol, int n = 1) {
    // TODO return the last n bars
} 

void SingleCSVDataHandler::updateBars() {
    // TODO push the latest bar onto the eventQueue
}

*/