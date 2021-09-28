#include "executionHandler.hpp"

InstantExecutionHandler::InstantExecutionHandler(std::queue<std::shared_ptr<Event>>* eventQueue, SingleCSVDataHandler* dataHandler) {
	this->eventQueue = eventQueue;
	this->dataHandler = dataHandler;
}

void InstantExecutionHandler::executeOrder(OrderEvent order) {
	auto timestamp = dataHandler->getLatestBars(order.symbol).rbegin()->first;

	eventQueue->push(std::make_shared<FillEvent>(order.symbol, timestamp, order.quantity, order.direction, 0, order.target));
}