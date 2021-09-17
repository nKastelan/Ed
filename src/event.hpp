#pragma once
#include <string>

class Event {
public:
    std::string type;
    virtual ~Event() = default;
};

class MarketEvent: public Event {
public:
    MarketEvent();
};

class SignalEvent: public Event {
public:
    std::string symbol;
    long timestamp;
    double signal;

    // symbol: symbol of the traded asset
    // timestamp: unix timestamp
    // signal: a double [-1, 1], negative for going short, 
    //         positive for going long, absolute value for signal strength

    SignalEvent(std::string symbol, long timestamp, double signal);
};

class OrderEvent: public Event {
public:
    std::string symbol;
    std::string order_type;
    double quantity;
    std::string direction;

    // symbol: symbol of the traded asset
    // order_type: "M" for market, "L" for limit
    // quantity: quantity we wish to trade
    // direction: "L" for long, "S" for short

    OrderEvent(std::string symbol, std::string order_type, double quantity, std::string direction);

    void logOrder();
};

class FillEvent: public Event {
public:
    std::string symbol;
    long timestamp;
    double quantity;
    std::string direction;
    double cost;
    double commission;
    double slippage;

    // symbol: symbol of the traded asset
    // timestamp: unix timestamp
    // quantity: quantity we wish to trade
    // direction: "L" for long, "S" for short
    // cost: cost of the filled order in $
    // commission: commission for the order in $, Binance spot trading 0.1%
    // slippage: slippage for the order as a percentage

    FillEvent(std::string symbol, long timestamp, double quantity, std::string direction, double cost);

    double calculateCommission();
    double calculateSlippage();
};