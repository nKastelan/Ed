#include "strategy.hpp"

TradingStrategy::TradingStrategy(SingleCSVDataHandler* dataHandler) {
	this->dataHandler = dataHandler;
	this->eventQueue = dataHandler->eventQueue;

	std::unordered_map<std::string, bool> bought;
	for (auto symbol : dataHandler->symbols) {
		bought[symbol] = false;
	}

	this->bought = bought;
}

void TradingStrategy::calculateSignals() {
	int n = 18;
	for (auto symbol : dataHandler->symbols) {
		int direction = 0;

		std::vector<double> closes;
		auto data = this->dataHandler->getLatestBars(symbol, n + 1);
		for (auto close : data) {
			closes.push_back(std::get<3>(close.second));
		}
		
		double rsi = 0;
		int outBeg;
		int outNbElement;
		
		auto retCode = TA_RSI(n, n, closes.data(), n, &outBeg, &outNbElement, &rsi);

		if (retCode != TA_SUCCESS) {
			return;
		}
		else {
			if (rsi > 70) {
				direction = -1;
			}
			else if (rsi < 30) {
				direction = 1;
			}
		}
		
		if (direction != 0 && ((direction == 1 && !bought[symbol]) || (direction == -1 && bought[symbol]))) {
			auto timestamp = dataHandler->consumedData.at(symbol).begin()->first;
			eventQueue->push(std::make_shared<SignalEvent>(symbol, timestamp, direction * 1.0, "ALGO"));
			bought[symbol] = !bought[symbol];
		}
	}
}