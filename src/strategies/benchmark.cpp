#include "strategy.hpp"

Benchmark::Benchmark(DataHandler* dataHandler) {
	this->dataHandler = dataHandler;
	this->eventQueue = dataHandler->eventQueue;
	this->bought = false;
}

void Benchmark::calculateSignals() {
	if (!bought) {
		auto timestamp = dataHandler->getLatestBars(dataHandler->symbol).at(dataHandler->symbol).begin()->first;
		eventQueue->push(SignalEvent(dataHandler->symbol, timestamp, 1.0));
		bought = true;
	}
}