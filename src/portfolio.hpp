#pragma once
#include "event.hpp"
#include "dataHandler.hpp"
#include "ta_libc.h"

class Portfolio {
public:
	virtual void onSignal(SignalEvent event) = 0;
	virtual void onFill(FillEvent event) = 0;
};

class SimplePortfolio: Portfolio {
public:
	// Pointer to the DataHandler used
	SingleCSVDataHandler* dataHandler;
	// Pointer to the queue of Events
	std::queue<Event*>* eventQueue;
	// Symbols of the traded assets
	std::vector<std::string> symbols;
	// Starting value of the Portfolio in $
	double* initialCapital;
	// All the previous positions captured at their respective timestamps
	std::map<long long, std::unordered_map<std::string, double>> allPositions;
	// All the current positions
	std::unordered_map<std::string, double> currentPositions;
	// All the previous holdings captured at their respective timestamps
	std::map<long long, std::unordered_map<std::string, double>> allHoldings;
	// All the current holdings
	std::unordered_map<std::string, double> currentHoldings;
	// Performance metrics
	std::unordered_map<std::string, double> performanceMetrics;

	SimplePortfolio(SingleCSVDataHandler* dataHandler, std::vector<std::string> symbols, double* initialCapital);

	SimplePortfolio() = default;

	// Constructs allPositions
	std::map<long long, std::unordered_map<std::string, double>> constructAllPositions();
	// Constructs currentPositions 
	std::unordered_map<std::string, double> constructCurrentPositions();

	// Constructs allHoldings
	std::map<long long, std::unordered_map<std::string, double>> constructAllHoldings();
	// Constructs currentHoldings
	std::unordered_map<std::string, double> constructCurrentHoldings();

	// Updates positions and holdings on every MarketEvent
	void update();

	// Updates positions based on FillEvents
	void updatePositionsOnFill(FillEvent event);
	// Updates holdings based on FillEvents
	void updateHoldingsOnFill(FillEvent event);

	// Generates orders based on SignalEvents
	void onSignal(SignalEvent event);
	// Updates portfolio based on FillEvents
	void onFill(FillEvent event);

	// Sends orders
	void generateOrder(SignalEvent event);

	// Calculates and returns the performance metrics
	void getMetrics();
};