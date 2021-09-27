#include "strategy.hpp"

Benchmark::Benchmark(SingleCSVDataHandler* dataHandler) {
	this->dataHandler = dataHandler;
	this->eventQueue = dataHandler->eventQueue;

	std::unordered_map<std::string, bool> bought;
	for (auto symbol : dataHandler->symbols) {
		bought[symbol] = false;
	}

	this->bought = bought;
}

void Benchmark::calculateSignals() {
	for (auto symbol : dataHandler->symbols) {
		if (!bought.at(symbol)) {
			auto timestamp = dataHandler->consumedData.at(symbol).begin()->first;
			eventQueue->push(new SignalEvent(symbol, timestamp, 1.0, "BENCH"));
			bought[symbol] = true;
		}
	}
}