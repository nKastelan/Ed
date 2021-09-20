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
	// TODO implement long then short every x days as a test
}