#include <iostream>
#include <chrono>
#include "dataHandler.hpp"

int main(int argc, char **argv) {

    std::queue<Event> eventQueue;
    bool continueBacktest = true;

    auto start = std::chrono::high_resolution_clock::now();
    auto dataHandler = SingleCSVDataHandler(&eventQueue, "data/Binance_ETHUSDT_1h.csv", "ETH/USDT", &continueBacktest);
    dataHandler.loadData();
    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Done,loading data into memory took " << time.count() << "ms" << std::endl;

    return 0;
}