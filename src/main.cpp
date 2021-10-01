#include <iostream>
#include <chrono>
#include "backtest.hpp"
#include "dataHandler.hpp"
#include "strategies/strategy.hpp"

int main(int argc, char **argv) {

    double initialCapital = 1000;
    std::vector<std::string> symbols;
    symbols.push_back("ETH/USDT");
    auto backtest = Backtest(symbols, "data/Binance_ETHUSDT_day.csv", &initialCapital);
    auto strategy = TradingStrategy(&backtest.dataHandler);
    auto benchmark = Benchmark(&backtest.dataHandler);
    
    auto retCode = TA_Initialize();
    if (retCode != TA_SUCCESS) {
        std::cout << "CANNOT INITIALIZE TA-LIB!" << std::endl;
        return -1;
    }
    

    auto start = std::chrono::high_resolution_clock::now();
    backtest.run(strategy, benchmark);
    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "\nDone, backtesting took " << time.count() << "ms" << std::endl;
    
    return TA_Shutdown();
}