#include "backtest.hpp"

Backtest::Backtest(std::vector<std::string> symbols, std::string csvDirectory, double* initialCapital) {
	this->symbols = symbols;
	this->csvDirectory = csvDirectory;
	this->initialCapital = initialCapital;
	this->continueBacktest = false;
	this->eventQueue;
	this->exchange;
	this->dataHandler = SingleCSVDataHandler(&eventQueue, csvDirectory, symbols, &continueBacktest);
	this->portfolio = SimplePortfolio(&dataHandler, &eventQueue, symbols, initialCapital);
	this->benchmarkDataHandler = SingleCSVDataHandler(&eventQueue, csvDirectory, symbols, &continueBacktest);
	this->benchmarkPortfolio = SimplePortfolio(&benchmarkDataHandler, &eventQueue, symbols, initialCapital);
}

void Backtest::run(TradingStrategy strategy, Benchmark benchmark) {
	continueBacktest = true;
	dataHandler.loadData();
	dataHandler.updateBars();
	benchmarkDataHandler.loadData();
	benchmarkDataHandler.updateBars();

	std::cout << "Backtest started!" << std::endl;

	while (continueBacktest) {

		if (!eventQueue.empty()) {
			auto event = eventQueue.front();
			
			if (event.type == "MARKET") {

			}
			else if (event.type == "SIGNAL") {

			}
			else if (event.type == "ORDER") {

			}
			else if (event.type == "FILL") {

			}

			eventQueue.pop();
		}
	}
}