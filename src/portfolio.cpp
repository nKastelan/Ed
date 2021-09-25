#include "portfolio.hpp"

SimplePortfolio::SimplePortfolio(SingleCSVDataHandler* dataHandler, std::vector<std::string> symbols, double* initialCapital) {
	this->dataHandler = dataHandler;
	this->eventQueue = dataHandler->eventQueue;
	this->symbols = symbols;
	this->initialCapital = initialCapital;
	this->allPositions = constructAllPositions();
	this->currentPositions = constructCurrentPositions();
	this->allHoldings = constructAllHoldings();
	this->currentHoldings = constructCurrentHoldings();
}

std::map<long long, std::unordered_map<std::string, double>> SimplePortfolio::constructAllPositions() {
	std::unordered_map<std::string, double> innerMap;
	for (auto symbol : symbols) {
		innerMap.insert(std::make_pair(symbol, 0.0));
	}
	auto firstTimestamp = dataHandler->bar->first;
	std::map<long long, std::unordered_map<std::string, double>> map;
	map.insert(std::make_pair(firstTimestamp, innerMap));
	return map;
}

std::unordered_map<std::string, double> SimplePortfolio::constructCurrentPositions() {
	std::unordered_map<std::string, double> map;
	for (auto symbol : symbols) {
		map.insert(std::make_pair(symbol, 0.0));
	}
	return map;
}

std::map<long long, std::unordered_map<std::string, double>> SimplePortfolio::constructAllHoldings() {
	std::unordered_map<std::string, double> innerMap;
	for (auto symbol : symbols) {
		innerMap.insert(std::make_pair(symbol, 0.0));
	}
	innerMap.insert(std::make_pair("cash", *initialCapital));
	innerMap.insert(std::make_pair("commission", 0.0));
	innerMap.insert(std::make_pair("slippage", 0.0));
	innerMap.insert(std::make_pair("total", *initialCapital));
	innerMap.insert(std::make_pair("returns", 0.0));
	innerMap.insert(std::make_pair("equityCurve", 0.0));
	auto firstTimestamp = dataHandler->bar->first;
	std::map<long long, std::unordered_map<std::string, double>> map;
	map.insert(std::make_pair(firstTimestamp, innerMap));
	return map;
}

std::unordered_map<std::string, double> SimplePortfolio::constructCurrentHoldings() {
	std::unordered_map<std::string, double> map;
	for (auto symbol : symbols) {
		map.insert(std::make_pair(symbol, 0.0));
	}
	map.insert(std::make_pair("cash", *initialCapital));
	map.insert(std::make_pair("commission", 0.0));
	map.insert(std::make_pair("slippage", 0.0));
	map.insert(std::make_pair("total", *initialCapital));
	return map;
}

void SimplePortfolio::update() {
	double notCash = 0.0;
	auto prevTotal = allHoldings.rbegin()->second["total"];
	auto prevEquityCurve = allHoldings.rbegin()->second["equityCurve"];
	auto timeStamp = dataHandler->getLatestBars(symbols.at(0)).begin()->first;
	std::unordered_map<std::string, std::map<long long, std::tuple<double, double, double, double, double>>> lastBar;
	for (auto symbol : symbols) {
		lastBar.insert(std::make_pair(symbol, dataHandler->getLatestBars(symbol)));
		allPositions[timeStamp][symbol] = currentPositions[symbol];
		auto currentValue = currentPositions.at(symbol) * lastBar.at(symbol).begin()->first;
		allHoldings[timeStamp][symbol] = currentValue;
		currentHoldings[symbol] = currentValue;
		notCash += currentValue;
	}

	currentHoldings["total"] = currentHoldings["cash"] + notCash;
	allHoldings[timeStamp]["total"] = currentHoldings["total"];
	allHoldings[timeStamp]["cash"] = currentHoldings["cash"];
	allHoldings[timeStamp]["commission"] = currentHoldings["commission"];
	allHoldings[timeStamp]["slippage"] = currentHoldings["slippage"];

	if (allHoldings.size()) {
		auto returns = (allHoldings[timeStamp]["total"] / prevTotal) - 1;
		allHoldings[timeStamp]["returns"] = returns;
		allHoldings[timeStamp]["equityCurve"] = (prevEquityCurve + 1) * (returns + 1) - 1;
	}
}

void SimplePortfolio::updatePositionsOnFill(FillEvent fill) {
	int direction = 0;
	if (fill.direction == "LONG") {
		direction = 1;
	} else if (fill.direction == "SHORT") {
		direction = -1;
	}

	currentPositions[fill.symbol] += direction * fill.quantity;
}

void SimplePortfolio::updateHoldingsOnFill(FillEvent fill) {
	int direction = 0;
	if (fill.direction == "LONG") {
		direction = 1;
	}
	else if (fill.direction == "SHORT") {
		direction = -1;
	}

	auto price = std::get<3>(dataHandler->getLatestBars(fill.symbol).rbegin()->second);
	auto cost = direction * fill.quantity * price;

	currentHoldings[fill.symbol] += cost;
	currentHoldings["total"] -= (fill.commission + fill.slippage);
	currentHoldings["cash"] -= (cost + fill.commission + fill.slippage);
	currentHoldings["commission"] += fill.commission;
	currentHoldings["slippage"] += fill.slippage;
}

void SimplePortfolio::onSignal(SignalEvent event) {
	generateOrder(event);
}

void SimplePortfolio::onFill(FillEvent event) {
	updateHoldingsOnFill(event);
	updatePositionsOnFill(event);
}

void SimplePortfolio::generateOrder(SignalEvent event) {
	double quantity = 10;
	std::string direction;
	if (event.signal > 0) {
		direction = "LONG";
	} else if (event.signal < 0) {
		direction = "SHORT";
	}

	eventQueue->push(new OrderEvent(event.symbol, "MARKET", quantity, direction, event.target));
}

void SimplePortfolio::getMetrics() {
	//
}