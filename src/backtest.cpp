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
				strategy.calculateSignals();
				benchmark.calculateSignals();
			}
			else if (event.type == "SIGNAL") {
				auto signal = dynamic_cast<SignalEvent*>(&event);
				if (signal->target == "ALGO") {
					portfolio.onSignal(*signal);
				}
				else if (signal->target == "BENCH") {
					benchmarkPortfolio.onSignal(*signal);
				}
			}
			else if (event.type == "ORDER") {
				auto order = dynamic_cast<OrderEvent*>(&event);
				exchange.executeOrder(*order);
				order->logOrder();
			}
			else if (event.type == "FILL") {
				auto fill = dynamic_cast<FillEvent*>(&event);
				if (fill->target == "ALGO") {
					portfolio.onFill(*fill);
				}
				else if (fill->target == "BENCH") {
					benchmarkPortfolio.onFill(*fill);
				}
			}

			eventQueue.pop();
		}
		else {
			portfolio.update();
			benchmarkPortfolio.update();
		}
	}
}