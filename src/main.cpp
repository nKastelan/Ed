#include <iostream>
#include <chrono>
#include "backtest.hpp"
#include "dataHandler.hpp"
#include "strategies/strategy.hpp"

int main(int argc, char **argv) {

    double initialCapital = 10000;
    std::vector<std::string> symbols;
    symbols.push_back("ETH/USDT");
    auto backtest = Backtest(symbols, "data/Binance_ETHUSDT_d.csv", &initialCapital);
    auto strategy = TradingStrategy(&backtest.dataHandler);
    auto benchmark = Benchmark(&backtest.dataHandler);


    auto start = std::chrono::high_resolution_clock::now();
    backtest.run(strategy, benchmark);
    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Done,backtesting took " << time.count() << "ms" << std::endl;
    

    return 0;
}