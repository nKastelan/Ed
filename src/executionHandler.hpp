#pragma once
#include "event.hpp"
#include "dataHandler.hpp"
#include <queue>

class ExecutionHandler {
public:
	std::queue<Event>* eventQueue;
	DataHandler* dataHandler;
	virtual void executeOrder(OrderEvent order) = 0;
};

class InstantExecutionHandler : ExecutionHandler {
public:
	InstantExecutionHandler(std::queue<Event>* eventQueue, DataHandler* dataHandler);

	InstantExecutionHandler();

	// Simulates the exchange filling the order
	void executeOrder(OrderEvent order);
};