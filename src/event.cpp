#include "event.hpp"

MarketEvent::MarketEvent() {
    type = "MARKET";
}

SignalEvent::SignalEvent(std::string symbol, long timestamp, double signal) {
    this->type = "SIGNAL";
    this->symbol = symbol;
    this->timestamp = timestamp;
    this->signal = signal;
}

OrderEvent::OrderEvent(std::string symbol, std::string order_type, double quantity, std::string direction) {
    this->type = "ORDER";
    this->symbol = symbol;
    this->order_type = order_type;
    this->quantity = quantity;
    this->direction = direction;
}

FillEvent::FillEvent(std::string symbol, long timestamp, double quantity, std::string direction, double cost) {
    this->type = "FILL";
    this->symbol = symbol;
    this->timestamp = timestamp;
    this->quantity = quantity;
    this->direction = direction;
    this->cost = cost;
    this->commission = calculateCommission();
    this->slippage = calculateSlippage();
}

//Returns the commission in $, Binance spot rate at 0.1%
double FillEvent::calculateCommission() {
    return 0.001 * cost;
}

//Returns the slippage in %, currently 0
double FillEvent::calculateSlippage() {
    return 0.0;
}