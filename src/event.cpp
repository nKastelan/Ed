#include "event.hpp"

MarketEvent::MarketEvent() {
    type = "MARKET";
}

SignalEvent::SignalEvent(std::string symbol, long long timestamp, double signal, std::string target) {
    this->type = "SIGNAL";
    this->symbol = symbol;
    this->timestamp = timestamp;
    this->signal = signal;
    this->target = target;
}

OrderEvent::OrderEvent(std::string symbol, std::string order_type, double quantity, std::string direction, std::string target) {
    this->type = "ORDER";
    this->symbol = symbol;
    this->order_type = order_type;
    this->quantity = quantity;
    this->direction = direction;
    this->target = target;
}

void OrderEvent::logOrder() {
    std::cout << "Order placed!" << std::endl;
}

FillEvent::FillEvent(std::string symbol, long long timestamp, double quantity, std::string direction, double cost, std::string target) {
    this->type = "FILL";
    this->symbol = symbol;
    this->timestamp = timestamp;
    this->quantity = quantity;
    this->direction = direction;
    this->cost = cost;
    this->commission = calculateCommission();
    this->slippage = calculateSlippage();
    this->target = target;
}

double FillEvent::calculateCommission() {
    return 0.001 * cost;
}

double FillEvent::calculateSlippage() {
    return 0.0;
}