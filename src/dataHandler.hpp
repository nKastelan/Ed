#pragma once
#include <unordered_map>
#include <queue>
#include <sstream>
#include <fstream>
#include <string>
#include <unordered_set>
#include "event.hpp"

class DataHandler {
public:
    //virtual std::unordered_map<std::string, std::unordered_map<long, std::tuple<double, double, double, double, double>>> getLatestBars(std::string symbol, int n = 1) = 0;
    //virtual void updateBars() = 0;
    virtual ~DataHandler() = default;
};

class SingleCSVDataHandler: DataHandler {
public:
    // Queue of Events
    std::queue<Event>* eventQueue;
    // Path to the csv data files
    std::string csvDirectory;
    // Symbol of the traded asset
    std::string symbol;
    // Flag variable
    bool* continueBacktest;
    // Historical data in format <symbol, <timestamp, [open, high, low, close, volume]>>
    std::unordered_map<std::string, std::unordered_map<long, std::tuple<double, double, double, double, double>>> data;

    SingleCSVDataHandler(std::queue<Event>* eventQueue, std::string csvDirectory, std::string symbol, bool* continueBacktest);

    // Formats and loads the data into memory ("data")
    void loadData();

    // Returns the "n" latest bars in format <symbol, <timestamp, [open, high, low, close, volume]>>
    //std::unordered_map<std::string, std::unordered_map<long, std::tuple<double, double, double, double, double>>> getLatestBars(std::string symbol, int n);

    // Pushes the latest bar onto the "eventQueue"
    //void updateBars();
};