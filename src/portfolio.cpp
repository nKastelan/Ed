#include "portfolio.hpp"

SimplePortfolio::SimplePortfolio(SingleCSVDataHandler* dataHandler, std::vector<std::string>* symbols, double* initialCapital) {
	this->dataHandler = dataHandler;
	this->eventQueue = dataHandler->eventQueue;
	this->symbols = *symbols;
	this->initialCapital = initialCapital;
	this->allPositions = constructAllPositions();
	this->currentPositions = constructCurrentPositions();
	this->allHoldings = constructAllHoldings();
	this->currentHoldings = constructCurrentHoldings();
}

std::map<long long, std::unordered_map<std::string, double>> SimplePortfolio::constructAllPositions() {
	std::unordered_map<std::string, double> innerMap;
	for (auto symbol : symbols) {
		innerMap.insert({ symbol, 0.0 });
	}
	auto firstTimestamp = dataHandler->bar->first;
	std::map<long long, std::unordered_map<std::string, double>> map;
	map.insert({ firstTimestamp, innerMap });
	return map;
}

std::unordered_map<std::string, double> SimplePortfolio::constructCurrentPositions() {
	std::unordered_map<std::string, double> map;
	for (auto symbol : symbols) {
		map.insert({ symbol, 0.0 });
	}
	return map;
}

std::map<long long, std::unordered_map<std::string, double>> SimplePortfolio::constructAllHoldings() {
	std::unordered_map<std::string, double> innerMap;
	for (auto symbol : symbols) {
		innerMap.insert({ symbol, 0.0 });
	}
	innerMap.insert({ "cash", *initialCapital });
	innerMap.insert({ "commission", 0.0 });
	innerMap.insert({ "slippage", 0.0 });
	innerMap.insert({ "total", *initialCapital });
	innerMap.insert({ "returns", 0.0 });
	innerMap.insert({ "equityCurve", 0.0 });
	auto firstTimestamp = dataHandler->bar->first;
	std::map<long long, std::unordered_map<std::string, double>> map;
	map.insert({ firstTimestamp, innerMap });
	return map;
}

std::unordered_map<std::string, double> SimplePortfolio::constructCurrentHoldings() {
	std::unordered_map<std::string, double> map;
	for (auto symbol : symbols) {
		map.insert({ symbol, 0.0 });
	}
	map.insert({ "cash", *initialCapital });
	map.insert({ "commission", 0.0 });
	map.insert({ "slippage", 0.0 });
	map.insert({ "total", *initialCapital });
	return map;
}

void SimplePortfolio::update() {
	auto notCash = 0.0;
	auto prevTotal = allHoldings.rbegin()->second["total"];
	auto prevEquityCurve = allHoldings.rbegin()->second["equityCurve"];
	auto timeStamp = dataHandler->consumedData[symbols[0]].rbegin()->first;
	for (auto symbol : symbols) {
		allPositions[timeStamp][symbol] = currentPositions[symbol];
		auto price = std::get<3>(dataHandler->consumedData[symbol].rbegin()->second);
		auto currentValue = currentPositions[symbol] * price;
		allHoldings[timeStamp][symbol] = currentValue;
		currentHoldings[symbol] = currentValue;
		notCash += currentValue;
	}

	currentHoldings["total"] = currentHoldings["cash"] + notCash;
	allHoldings[timeStamp]["total"] = currentHoldings["total"];
	allHoldings[timeStamp]["cash"] = currentHoldings["cash"];
	allHoldings[timeStamp]["commission"] = currentHoldings["commission"];
	allHoldings[timeStamp]["slippage"] = currentHoldings["slippage"];

	if (allHoldings.size() > 1) {
		auto returns = (allHoldings[timeStamp]["total"] / prevTotal) - 1;
		allHoldings[timeStamp]["returns"] = returns;
		allHoldings[timeStamp]["equityCurve"] = (prevEquityCurve + 1) * (returns + 1) - 1;
	}
}

void SimplePortfolio::updatePositionsOnFill(std::shared_ptr<FillEvent> fill) {
	int direction = 0;
	if (fill->direction == "LONG") {
		direction = 1;
	} else if (fill->direction == "SHORT") {
		direction = -1;
	}

	currentPositions[fill->symbol] += direction * fill->quantity;
}

void SimplePortfolio::updateHoldingsOnFill(std::shared_ptr<FillEvent> fill) {
	int direction = 0;
	if (fill->direction == "LONG") {
		direction = 1;
	}
	else if (fill->direction == "SHORT") {
		direction = -1;
	}

	auto price = std::get<3>(dataHandler->consumedData.at(fill->symbol).rbegin()->second);
	auto cost = direction * fill->quantity * price;

	currentHoldings[fill->symbol] += cost;
	currentHoldings["total"] -= (fill->commission + fill->slippage);
	currentHoldings["cash"] -= (cost + fill->commission + fill->slippage);
	currentHoldings["commission"] += fill->commission;
	currentHoldings["slippage"] += fill->slippage;
}

void SimplePortfolio::onSignal(std::shared_ptr<SignalEvent> event) {
	generateOrder(event);
}

void SimplePortfolio::onFill(std::shared_ptr<FillEvent> event) {
	updateHoldingsOnFill(event);
	updatePositionsOnFill(event);
}

void SimplePortfolio::generateOrder(std::shared_ptr<SignalEvent> event) {
	auto quantity = 1.0;
	/*
	if (event->target == "ALGO") {
		quantity = 1;
	}
	else {
		quantity = getMaxQuantity(event);
	}
	*/
	if (quantity == 0) return;
	
	std::string direction;
	if (event->signal > 0) {
		direction = "LONG";
	} else if (event->signal < 0) {
		direction = "SHORT";
	}

	eventQueue->push(std::make_shared<OrderEvent>(&event->symbol, "MARKET", &quantity, &direction, event->target));
}

double SimplePortfolio::getMaxQuantity(std::shared_ptr<SignalEvent> event) {
	auto cash = currentHoldings.at("cash");
	auto price = std::get<3>(dataHandler->consumedData.at(event->symbol).rbegin()->second);
	return cash / price;
}

void SimplePortfolio::getMetrics() {
	auto rit = dataHandler->consumedData[symbols[0]].rbegin();
	// unix difference between two data points, needed for proper annualization
	auto diff = rit->first - (++rit)->first;
	// 86'400'000 for daily data
	// 3'600'000 for hourly data
	// 60'000 for minute data

	int period = 0;
	switch (diff) {
	case 86400000:
		period = 365;
		break; 

	case 3600000: 
		period = 365*24;
		break;
		
	case 60000: 
		period = 365*24*60;
		break;
		
	}

	
	double mean = std::accumulate(allHoldings.begin(), allHoldings.end(), 0.0, 
		[](double value, const std::map<long long, std::unordered_map<std::string, double>>::value_type& p) 
		{return value + p.second.at("returns"); });
	mean /= allHoldings.size();
	performanceMetrics["Return (Avg. per Bar) [%]"] = mean * 100;
	performanceMetrics["Return (Avg. Daily) [%]"] = (pow(mean + 1, period / 365) - 1) * 100;

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
	performanceMetrics["Total Return [%]"] = returns;

	double exposure = std::accumulate(allPositions.begin(), allPositions.end(), 0.0,
		[this](double value, const std::map<long long, std::unordered_map<std::string, double>>::value_type& p)
		{for (auto symbol : symbols) { if (p.second.at(symbol) != 0) return ++value; else return value; }});
	performanceMetrics["Exposure Time [%]"] = exposure / allPositions.size() * 100;


	for (auto metric : performanceMetrics) {
		std::cout << metric.first << ": " << metric.second << std::endl;
	}
}