#include "strategy.hpp"

TradingStrategy::TradingStrategy(DataHandler* dataHandler) {
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
	for (unsigned int i = 0; i < dataHandler->symbols.size(); ++i) {
		auto symbol = dataHandler->symbols.at(i);
		if (!bought.at(symbol)) {
			auto timestamp = dataHandler->getLatestBars(symbol).begin()->first;
			eventQueue->push(SignalEvent(symbol, timestamp, 1.0, "ALGO"));
			bought.insert_or_assign(symbol, true);
		}
	}
}