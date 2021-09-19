#include <iostream>
#include "dataHandler.hpp"

int main(int argc, char **argv) {

    std::queue<Event> eventQueue;
    bool continueBacktest = true;

    auto dataHandler = SingleCSVDataHandler(&eventQueue, "data/Binance_ETHUSDT_d.csv", "ETH/USDT", &continueBacktest);
    std::cout << "Done" << std::endl;

    return 0;
}