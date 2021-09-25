#include "backtest.hpp"

Backtest::Backtest(std::vector<std::string> symbols, std::string csvDirectory, double* initialCapital) : exchange(&eventQueue, &dataHandler) {
	this->symbols = symbols;
	this->csvDirectory = csvDirectory;
	this->initialCapital = initialCapital;
	this->continueBacktest = false; 
	//this->eventQueue = new std::queue<Event*>;
	this->dataHandler = SingleCSVDataHandler(&eventQueue, csvDirectory, symbols, &continueBacktest);
	this->portfolio = SimplePortfolio(&dataHandler, symbols, initialCapital);
	this->benchmarkDataHandler = SingleCSVDataHandler(&eventQueue, csvDirectory, symbols, &continueBacktest);
	this->benchmarkPortfolio = SimplePortfolio(&benchmarkDataHandler, symbols, initialCapital);
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
			
			if (event->type == "MARKET") {
				strategy.calculateSignals();
				benchmark.calculateSignals();
			}
			else if (event->type == "SIGNAL") {
				auto signal = dynamic_cast<SignalEvent*>(event);
				if (event->target == "ALGO") {
					portfolio.onSignal(*signal);
				}
				else if (event->target == "BENCH") {
					benchmarkPortfolio.onSignal(*signal);
				}
			}
			else if (event->type == "ORDER") {
				auto order = dynamic_cast<OrderEvent*>(event);
				exchange.executeOrder(*order);
				order->logOrder();
			}
			else if (event->type == "FILL") {
				auto fill = dynamic_cast<FillEvent*>(event);
				if (event->target == "ALGO") {
					portfolio.onFill(*fill);
				}
				else if (event->target == "BENCH") {
					benchmarkPortfolio.onFill(*fill);
				}
			}

			eventQueue.pop();
		}
		else {
			portfolio.update();
			dataHandler.updateBars();
			benchmarkPortfolio.update();
			benchmarkDataHandler.updateBars();
		}
	}
}