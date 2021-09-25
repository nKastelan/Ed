#pragma once
#include "dataHandler.hpp"
#include "executionHandler.hpp"
#include "portfolio.hpp"
#include "strategies/strategy.hpp"

class Backtest {
public:
	std::vector<std::string> symbols;
	std::string csvDirectory;
	double* initialCapital;
	bool continueBacktest;
	std::queue<Event*>* eventQueue;
	InstantExecutionHandler exchange;
	SimplePortfolio portfolio;
	SingleCSVDataHandler dataHandler;
	TradingStrategy strategy;
	SingleCSVDataHandler benchmarkDataHandler;
	SimplePortfolio benchmarkPortfolio;
	Benchmark benchmark;

	Backtest(std::vector<std::string> symbols, std::string csvDirectory, double* initialCapital);

	// Runs the backtest!
	void run(TradingStrategy strategy, Benchmark benchmark);
};