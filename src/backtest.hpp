#pragma once
#include "dataHandler.hpp"
#include "executionHandler.hpp"
#include "portfolio.hpp"
#include "strategies/strategy.hpp"
#include <future>

class Backtest {
public:
	std::vector<std::string> symbols;
	std::string csvDirectory;
	double* initialCapital;
	bool continueBacktest;
	std::queue<std::shared_ptr<Event>> eventQueue;
	InstantExecutionHandler exchange;
	SimplePortfolio portfolio;
	SingleCSVDataHandler dataHandler;
	TradingStrategy strategy;
	SimplePortfolio benchmarkPortfolio;
	Benchmark benchmark;

	Backtest(std::vector<std::string>* symbols, std::string* csvDirectory, double* initialCapital);

	// Runs the backtest!
	void run(TradingStrategy* strategy, Benchmark* benchmark);
};