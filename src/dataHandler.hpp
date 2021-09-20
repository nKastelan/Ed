#pragma once
#include <unordered_map>
#include <map>
#include <queue>
#include <sstream>
#include <fstream>
#include <string>
#include "event.hpp"

class DataHandler {
public:
    // Queue of Events
    std::queue<Event>* eventQueue = 0;
    // Flag variable
    bool* continueBacktest = 0;
    // Symbols of the traded assets
    std::vector<std::string> symbols;

    virtual std::unordered_map<std::string, std::map<long long, std::tuple<double, double, double, double, double>>> getLatestBars(std::string symbol, int n = 1) = 0;
    virtual void updateBars() = 0;
    virtual ~DataHandler() = default;
};

class SingleCSVDataHandler: public DataHandler {
public:
    // Path to the csv data files
    std::string csvDirectory;
    // Historical data in format <symbol, <timestamp, [open, high, low, close, volume]>>
    std::unordered_map<std::string, std::map<long long, std::tuple<double, double, double, double, double>>> data;
    // Data consumed so far in format <symbol, <timestamp, [open, high, low, close, volume]>>
    std::unordered_map<std::string, std::map<long long, std::tuple<double, double, double, double, double>>> consumedData;
    // Iterator over the historical data contained in "data"
    std::map<long long, std::tuple<double, double, double, double, double>>::iterator bar;

    SingleCSVDataHandler(std::queue<Event>* eventQueue, std::string csvDirectory, std::vector<std::string> symbol, bool* continueBacktest);

    // Formats and loads the data into memory ("data")
    void loadData();

    // Returns the "n" latest bars in format <symbol, <timestamp, [open, high, low, close, volume]>>
    std::unordered_map<std::string, std::map<long long, std::tuple<double, double, double, double, double>>> getLatestBars(std::string symbol, int n = 1);

    // Pushes the latest bar onto the "eventQueue"
    void updateBars();
};