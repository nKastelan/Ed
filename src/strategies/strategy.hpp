#pragma once
#include "../event.hpp"
#include "../dataHandler.hpp"
#include "ta_libc.h"

class Strategy {
public:
	// Creates SignalEvents based on the data
	virtual void calculateSignals() = 0;
};

class Benchmark: Strategy {
public:
	// Pointer to the DataHandler used
	DataHandler* dataHandler;
	// Pointer to the eventQueue, accesible with dataHandler.eventQueue
	std::queue<Event>* eventQueue;
	// Flag variable to indicate if the assets are bought
	std::unordered_map<std::string, bool> bought;

	Benchmark(DataHandler* dataHandler);

	Benchmark();

	// Puts a SignalEvent onto the eventQueue after the first bar
	void calculateSignals();
};

class TradingStrategy : Strategy {
public:
	// Pointer to the DataHandler used
	DataHandler* dataHandler;
	// Pointer to the eventQueue, accesible with dataHandler.eventQueue
	std::queue<Event>* eventQueue;
	// Flag variable to indicate if the assets are bought
	std::unordered_map<std::string, bool> bought;

	TradingStrategy(DataHandler* dataHandler);

	TradingStrategy();

	// Puts SignalEvents onto the eventQueue
	void calculateSignals();
};