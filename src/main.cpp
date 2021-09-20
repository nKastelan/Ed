#include <iostream>
#include <chrono>
#include "dataHandler.hpp"
#include "strategies/strategy.hpp"

int main(int argc, char **argv) {

    std::queue<Event> eventQueue;
    bool continueBacktest = true;
    std::vector<std::string> symbols;
    symbols.push_back("ETH/USDT");

    auto start = std::chrono::high_resolution_clock::now();
    auto dataHandler = SingleCSVDataHandler(&eventQueue, "data/Binance_ETHUSDT_d.csv", symbols, &continueBacktest);
    dataHandler.loadData();
    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Done,loading data into memory took " << time.count() << "ms" << std::endl;
    dataHandler.updateBars();
    dataHandler.updateBars();
    dataHandler.updateBars();
    auto bars = dataHandler.getLatestBars(symbols[0], 2);
    std::cout << "Returned number of bars is " << bars.size() << std::endl;
    auto benchmark = Benchmark(&dataHandler);
    benchmark.calculateSignals();
    std::cout << benchmark.bought[symbols[0]] << std::endl;

    return 0;
}