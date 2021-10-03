#pragma once
#include "event.hpp"
#include "dataHandler.hpp"
#include <queue>

class ExecutionHandler {
public:
	std::queue<std::shared_ptr<Event>>* eventQueue;
	SingleCSVDataHandler* dataHandler;
	virtual void executeOrder(std::shared_ptr<OrderEvent> order) = 0;
};

class InstantExecutionHandler: ExecutionHandler {
public:
	InstantExecutionHandler(std::queue<std::shared_ptr<Event>>* eventQueue, SingleCSVDataHandler* dataHandler);

	InstantExecutionHandler() = default;

	// Simulates the exchange filling the order
	void executeOrder(std::shared_ptr<OrderEvent> order);
};