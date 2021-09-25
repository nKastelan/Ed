#include "strategy.hpp"

TradingStrategy::TradingStrategy(SingleCSVDataHandler* dataHandler) {
	this->dataHandler = dataHandler;
	this->eventQueue = dataHandler->eventQueue;

	std::unordered_map<std::string, bool> bought;
	auto size = dataHandler->symbols.size();
	for (unsigned int i = 0; i < size; ++i) {
		bought.insert(std::make_pair(dataHandler->symbols.at(i), false));
	}

	this->bought = bought;
}

void TradingStrategy::calculateSignals() {
	for (auto symbol : this->dataHandler->symbols) {
		if (!this->bought.at(symbol)) {
			auto timestamp = this->dataHandler->getLatestBars(symbol).begin()->first;
			this->eventQueue->push(new SignalEvent(symbol, timestamp, 1.0, "ALGO"));
			bought[symbol] = true;
		}
	}
}