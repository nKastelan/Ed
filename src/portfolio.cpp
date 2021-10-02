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
	auto notCash = 0.0;
	auto prevTotal = allHoldings.rbegin()->second["total"];
	auto prevEquityCurve = allHoldings.rbegin()->second["equityCurve"];
	auto timeStamp = dataHandler->consumedData.at(symbols[0]).rbegin()->first;
	for (auto symbol : symbols) {
		allPositions[timeStamp][symbol] = currentPositions[symbol];
		auto price = std::get<3>(dataHandler->consumedData.at(symbol).rbegin()->second);
		auto currentValue = currentPositions.at(symbol) * price;
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

	auto price = std::get<3>(dataHandler->consumedData.at(fill.symbol).rbegin()->second);
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
	auto quantity = getMaxQuantity(&event);
	if (quantity == 0) return;
	
	std::string direction;
	if (event.signal > 0) {
		direction = "LONG";
	} else if (event.signal < 0) {
		direction = "SHORT";
	}

	eventQueue->push(std::make_shared<OrderEvent>(event.symbol, "MARKET", quantity, direction, event.target));
}

double SimplePortfolio::getMaxQuantity(SignalEvent* event) {
	auto cash = currentHoldings.at("cash");
	auto price = std::get<3>(dataHandler->consumedData.at(event->symbol).rbegin()->second);
	return cash / price;
}

void SimplePortfolio::getMetrics() {
	auto rit = dataHandler->consumedData.at(symbols[0]).rbegin();
	// unix difference between two data points, needed for proper annualization
	auto diff = rit->first - (++rit)->first;
	// 86'400'000 for daily data
	// 3'600'000 for hourly data
	// 60'000 for minute data

	int period = 0;
	switch (diff) {
	case 86'400'000:
		period = 365;
		break; 

	case 3'600'000: 
		period = 365*24;
		break;
		
	case 60'000: 
		period = 365*24*60;
		break;
		
	}

	
	double mean = std::accumulate(allHoldings.begin(), allHoldings.end(), 0.0, 
		[](double value, const std::map<long long, std::unordered_map<std::string, double>>::value_type& p) 
		{return value + p.second.at("returns");});
	mean /= allHoldings.size();
	performanceMetrics["Return (Avg. Daily) [%]"] = mean * (period / 365) * 100;

	double std = std::accumulate(allHoldings.begin(), allHoldings.end(), 0.0,
		[](double value, const std::map<long long, std::unordered_map<std::string, double>>::value_type& p)
		{return value + pow(p.second.at("returns"), 2); });
	std /= allHoldings.size() - 1;
	std = sqrt(std);
	performanceMetrics["Volatility (Daily) [%]"] = std * sqrt(period / 365) * 100;

	
	double negativeBars = std::accumulate(allHoldings.begin(), allHoldings.end(), 0.0,
		[](double value, const std::map<long long, std::unordered_map<std::string, double>>::value_type& p)
		{if (p.second.at("returns") < 0) return ++value; else return value; });
	performanceMetrics["Positive Bars [%]"] = (allHoldings.size() - negativeBars) / allHoldings.size() * 100;
	
	double stdDownside = std::accumulate(allHoldings.begin(), allHoldings.end(), 0.0,
		[](double value, const std::map<long long, std::unordered_map<std::string, double>>::value_type& p)
		{if (p.second.at("returns") < 0) return value + pow(p.second.at("returns"), 2); else return value; });
	stdDownside /= negativeBars - 1;
	stdDownside = sqrt(stdDownside);
	performanceMetrics["Volatility Downside (Daily) [%]"] = stdDownside * sqrt(period / 365) * 100;
	
	double annSharpe = sqrt(period) * mean / std;
	performanceMetrics["Sharpe (Ann.)"] = annSharpe;
	
	double annSortino = sqrt(period) * mean / stdDownside;
	performanceMetrics["Sortino (Ann.)"] = annSortino;

	double equityPeak = std::max_element(allHoldings.begin(), allHoldings.end(), 
		[](const std::map<long long, std::unordered_map<std::string, double>>::value_type& p1,
			const std::map<long long, std::unordered_map<std::string, double>>::value_type& p2)
		{return p1.second.at("total") < p2.second.at("total"); })->second.at("total");
	performanceMetrics["Equity Peak [$]"] = equityPeak;

	double equityBottom = std::min_element(allHoldings.begin(), allHoldings.end(),
		[](const std::map<long long, std::unordered_map<std::string, double>>::value_type& p1,
			const std::map<long long, std::unordered_map<std::string, double>>::value_type& p2)
		{return p1.second.at("total") < p2.second.at("total"); })->second.at("total");
	performanceMetrics["Equity Bottom [$]"] = equityBottom;

	double equityFinal = allHoldings.rbegin()->second.at("total");
	performanceMetrics["Equity Final [$]"] = equityFinal;

	double years = (double)allHoldings.size() / period;
	performanceMetrics["Duration [years]"] = years;

	performanceMetrics["CAGR [%]"] = (pow(equityFinal / *initialCapital, 1 / years) - 1) * 100;

	double returns = equityFinal / *initialCapital * 100;
	performanceMetrics["Return [%]"] = returns;

	double exposure = std::accumulate(allPositions.begin(), allPositions.end(), 0.0,
		[this](double value, const std::map<long long, std::unordered_map<std::string, double>>::value_type& p)
		{for (auto symbol : symbols) { if (p.second.at(symbol) != 0) return ++value; else return value; }});
	performanceMetrics["Exposure Time [%]"] = exposure / allPositions.size() * 100;


	for (auto metric : performanceMetrics) {
		std::cout << metric.first << ": " << metric.second << std::endl;
	}
}