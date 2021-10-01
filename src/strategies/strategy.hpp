#pragma once
#include "../event.hpp"
#include "../dataHandler.hpp"
#include "ta_libc.h"
#include <vector>

class Strategy {
public:
	// Creates SignalEvents based on the data
	virtual void calculateSignals() = 0;
};

class Benchmark: Strategy {
public:
	// Pointer to the DataHandler used
	SingleCSVDataHandler* dataHandler;
	// Pointer to the eventQueue, accesible with dataHandler.eventQueue
	std::queue<std::shared_ptr<Event>>* eventQueue;
	// Flag variable to indicate if the assets are bought
	std::unordered_map<std::string, bool> bought;

	Benchmark(SingleCSVDataHandler* dataHandler);

	Benchmark() = default;

	// Puts a SignalEvent onto the eventQueue after the first bar
	void calculateSignals();
};

class TradingStrategy : Strategy {
public:
	// Pointer to the DataHandler used
	SingleCSVDataHandler* dataHandler;
	// Pointer to the eventQueue, accesible with dataHandler.eventQueue
	std::queue<std::shared_ptr<Event>>* eventQueue;
	// Flag variable to indicate if the assets are bought
	std::unordered_map<std::string, bool> bought;

	TradingStrategy(SingleCSVDataHandler* dataHandler);

	TradingStrategy() = default;

	// Puts SignalEvents onto the eventQueue
	void calculateSignals();
};