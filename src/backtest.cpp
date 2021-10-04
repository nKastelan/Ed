#include "backtest.hpp"

Backtest::Backtest(std::vector<std::string>* symbols, std::string* csvDirectory, double* initialCapital) : exchange(&eventQueue, &dataHandler) {
	this->symbols = *symbols;
	this->csvDirectory = *csvDirectory;
	this->initialCapital = initialCapital;
	this->continueBacktest = false; 
	this->dataHandler = SingleCSVDataHandler(&eventQueue, csvDirectory, symbols, &continueBacktest);
	this->portfolio = SimplePortfolio(&dataHandler, symbols, initialCapital);
	this->benchmarkPortfolio = SimplePortfolio(&dataHandler, symbols, initialCapital);
}

void Backtest::run(TradingStrategy* strategy, Benchmark* benchmark) {
	continueBacktest = true;
	bool first = true;
	dataHandler.loadData();
	dataHandler.updateBars();

	std::cout << "Backtest started!" << std::endl;

	while (continueBacktest) {

		while (!eventQueue.empty()) {
			auto event = eventQueue.front();
			eventQueue.pop();

			switch (event->type) {

			case 0: {
				strategy->calculateSignals();
				if (first) {
					benchmark->calculateSignals();
					first = false;
				}
				portfolio.update();
				benchmarkPortfolio.update();
				break;
				}

			case 1: {
				auto signal = dynamic_pointer_cast<SignalEvent>(event);
				if (event->target == "ALGO") {
					portfolio.onSignal(signal);
				}
				else if (event->target == "BENCH") {
					benchmarkPortfolio.onSignal(signal);
				}
				break;
				}

			case 2: {
				auto order = dynamic_pointer_cast<OrderEvent>(event);
				exchange.executeOrder(order);
				order->logOrder();
				break;
				}

			case 3: {
				auto fill = dynamic_pointer_cast<FillEvent>(event);
				if (event->target == "ALGO") {
					portfolio.onFill(fill);
					portfolio.update();
				}
				else if (event->target == "BENCH") {
					benchmarkPortfolio.onFill(fill);
					benchmarkPortfolio.update();
				}
				break;
				}
			}
		}

		dataHandler.updateBars();
	}

	std::cout << "Backtest ended!\n" << std::endl;

	std::cout << "PERFORMANCE METRICS (TRADING STRATEGY)\n--------------------------------------" << std::endl;
	portfolio.getMetrics();
	std::cout << "\n\nPERFORMANCE METRICS (BUY AND HOLD)\n----------------------------------" << std::endl;
	benchmarkPortfolio.getMetrics();
}