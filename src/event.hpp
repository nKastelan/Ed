#pragma once
#include <string>
#include <iostream>

class Event {
public:
    // event type
    std::string type;
    virtual ~Event() = default;
};

class MarketEvent: public Event {
public:
    MarketEvent();
};

class SignalEvent: public Event {
public:
    // Symbol of the traded asset
    std::string symbol;
    // Unix timestamp
    long long timestamp;
    // Range of [-1, 1], negative for going short, positive for going long, absolute value for signal strength
    double signal;

    SignalEvent(std::string symbol, long long timestamp, double signal);
};

class OrderEvent: public Event {
public:
    // Symbol of the traded asset
    std::string symbol;
    // "M" for market, "L" for limit
    std::string order_type;
    // Quantity we wish to trade
    double quantity;
    // "L" for long, "S" for short
    std::string direction;

    OrderEvent(std::string symbol, std::string order_type, double quantity, std::string direction);

    // Logs the order
    void logOrder();
};

class FillEvent: public Event {
public:
    // Symbol of the traded asset
    std::string symbol;
    // Unix timestamp
    long long timestamp;
    // Quantity we wish to trade
    double quantity;
    // "L" for long, "S" for short
    std::string direction;
    // Cost of the filled order in $
    double cost;
    // Commission for the order in $, Binance spot trading 0.1%
    double commission;
    // Slippage for the order as a percentage
    double slippage;

    FillEvent(std::string symbol, long long timestamp, double quantity, std::string direction, double cost);

    // Calculates the commission for the order in $, Binance spot 0.1%
    double calculateCommission();

    // Calculates the slippage of the order in %, currently not functional
    double calculateSlippage();
};