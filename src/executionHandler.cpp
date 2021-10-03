#include "executionHandler.hpp"

InstantExecutionHandler::InstantExecutionHandler(std::queue<std::shared_ptr<Event>>* eventQueue, SingleCSVDataHandler* dataHandler) {
	this->eventQueue = eventQueue;
	this->dataHandler = dataHandler;
}

void InstantExecutionHandler::executeOrder(std::shared_ptr<OrderEvent> order) {
	auto timestamp = dataHandler->bar->first;

	eventQueue->push(std::make_shared<FillEvent>(&order->symbol, &timestamp, &order->quantity, order->direction, 0, order->target));
}