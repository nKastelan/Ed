#pragma once
#include "../event.hpp"
#include "../dataHandler.hpp"

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
	// Flag variable to indicate if the asset is bought
	bool bought;

	Benchmark(DataHandler* dataHandler);

	// Puts a SignalEvent onto the eventQueue after the first bar
	void calculateSignals();
};