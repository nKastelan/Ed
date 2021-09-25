#include "strategy.hpp"

Benchmark::Benchmark(SingleCSVDataHandler* dataHandler) {
	this->dataHandler = dataHandler;
	this->eventQueue = dataHandler->eventQueue;

	std::unordered_map<std::string, bool> bought;
	auto size = dataHandler->symbols.size();
	for (unsigned int i = 0; i < size; ++i) {
		bought.insert(std::make_pair(dataHandler->symbols.at(i), false));
	}

	this->bought = bought;
}

void Benchmark::calculateSignals() {
	for (auto symbol : this->dataHandler->symbols) {
		if (!bought.at(symbol)) {
			auto timestamp = dataHandler->getLatestBars(symbol).begin()->first;
			this->eventQueue->push(new SignalEvent(symbol, timestamp, 1.0, "BENCH"));
			bought[symbol] = true;
		}
	}
}