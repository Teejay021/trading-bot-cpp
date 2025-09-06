#include "strategy/strategy.h"
#include <stdexcept>
#include <iostream>


TradingBot::RSIStrategy::RSIStrategy() : Strategy("RSI_STRATEGY") {
    rsi_period_ = 14;  // Default RSI period
    //Default overbought and oversold thresholds
    overbought_threshold_ = 70.0;  
    oversold_threshold_ = 30.0;   
}

// Initialize strategy with parameters
bool TradingBot::RSIStrategy::initialize(const std::map<std::string, double>& params) {
    auto period_it = params.find("period");
    auto overbought_threshold_it = params.find("overbought_threshold");
    auto oversold_threshold_it = params.find("oversold_threshold");

    if(period_it == params.end() || overbought_threshold_it == params.end() || oversold_threshold_it == params.end()){
        throw std::invalid_argument("Missing required parameters");
    }
    
    rsi_period_ = static_cast<int>(period_it->second);
    overbought_threshold_ = overbought_threshold_it->second;
    oversold_threshold_ = oversold_threshold_it->second;
    
    return true;
}

// Generate trading signal based on current market data
TradingBot::TradingSignal TradingBot::RSIStrategy::generate_signal(const MarketData& data, const Position& current_position) {
    TradingSignal signal;
    signal.type = SignalType::HOLD;
    signal.price = data.close;
    signal.timestamp = data.timestamp;
    
    price_history_.push_back(data);

    if(price_history_.size() > static_cast<size_t>(rsi_period_ + 1)){
        price_history_.erase(price_history_.begin());
    }

    double rsi = calculate_rsi(price_history_, rsi_period_);

    if(rsi < oversold_threshold_){
        signal.type = SignalType::BUY;
        signal.price = data.close;
        signal.quantity = 100.0; // Will be adjusted by risk management
        signal.reason = "RSI below oversold threshold";
    }
    else if(rsi > overbought_threshold_){
        signal.type = SignalType::SELL;
        signal.price = data.close;
        signal.quantity = current_position.quantity;
        signal.reason = "RSI above overbought threshold";
    }
    else{
        signal.reason = "RSI is between oversold and overbought thresholds";
    }
    
    return signal;
}

// Get current strategy parameters
std::map<std::string, double> TradingBot::RSIStrategy::get_parameters() const {
    std::map<std::string, double> params{
        {"period", static_cast<double>(rsi_period_)},
        {"overbought_threshold", overbought_threshold_},
        {"oversold_threshold", oversold_threshold_}
    };
    
    return params;
}


bool TradingBot::RSIStrategy::validate_parameters(const std::map<std::string, double>& params) const {

    auto period_it = params.find("period");
    auto overbought_threshold_it = params.find("overbought_threshold");
    auto oversold_threshold_it = params.find("oversold_threshold");

    if(period_it == params.end() || overbought_threshold_it == params.end() || oversold_threshold_it == params.end()){
        return false;
    }

    if(period_it->second <= 1 || period_it-> second > 50){
        return false;
    }

    if(overbought_threshold_it -> second < 50 ||   overbought_threshold_it -> second > 100){

        return false;
    }

    if(oversold_threshold_it -> second < 0 || oversold_threshold_it -> second > 50){
        return false;
    }
    
    if(overbought_threshold_it->second <= oversold_threshold_it->second){
        return false;
    }
    
    return true;
}
