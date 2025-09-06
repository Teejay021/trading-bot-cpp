#include "strategy/strategy.h"
#include <stdexcept>
#include <iostream>


TradingBot::EMAStrategy::EMAStrategy() : Strategy("EMA_STRATEGY") {
    //Common EMA periods used in MACD indicator
    short_period_ = 12;
    long_period_ = 26 ;
    
}


bool TradingBot::EMAStrategy::initialize(const std::map<std::string, double>& params) {
    auto short_period_it = params.find("short_period");
    auto long_period_it = params.find("long_period");

    if(short_period_it == params.end() || long_period_it == params.end()){
        return false;
    }

    short_period_ = static_cast<int>(short_period_it->second);
    long_period_ = static_cast<int>(long_period_it->second);

    
    return true;
}


TradingBot::TradingSignal TradingBot::EMAStrategy::generate_signal(const MarketData& data, const Position& current_position) {    
    TradingSignal signal;
    signal.type = SignalType::HOLD;
    signal.price = data.close;
    signal.timestamp = data.timestamp;
    signal.reason = "EMA strategy not implemented yet";

    price_history_.push_back(data);

    if (price_history_.size() < static_cast<size_t>(long_period_)) {
        return signal;
    }

    double short_ema = calculate_ema(price_history_, short_period_);
    double long_ema = calculate_ema(price_history_, long_period_);

    if(price_history_.size() > static_cast<size_t>(long_period_)){

        std::vector<MarketData> prev_data(price_history_.begin(), price_history_.end() - 1);

        double prev_short_ema = calculate_ema(prev_data, short_period_);
        double prev_long_ema = calculate_ema(prev_data, long_period_);

        if(prev_short_ema <= prev_long_ema && short_ema > long_ema){
            signal.type = SignalType::BUY;
            signal.price = data.close;
            signal.quantity = 100.0;
            signal.reason = "Short EMA crossed above long EMA";
        }
        else if(prev_short_ema >= prev_long_ema && short_ema < long_ema){
            signal.type = SignalType::SELL;
            signal.price = data.close;
            signal.quantity = current_position.quantity;
            signal.reason = "Short EMA crossed below long EMA";
        }
        else{
            signal.reason = "No crossover detected";
        }
    }
    
    return signal;
}


std::map<std::string, double> TradingBot::EMAStrategy::get_parameters() const {
    
    return std::map<std::string, double> {
        {"short_period", static_cast<double>(short_period_)},
        {"long_period", static_cast<double>(long_period_)}
    };
        
}

bool TradingBot::EMAStrategy::validate_parameters(const std::map<std::string, double>& params) const {
    auto short_period_it = params.find("short_period");
    auto long_period_it = params.find("long_period");

    if(short_period_it == params.end() || long_period_it == params.end()){
        return false;
    }

    if(short_period_it->second <= 1 || short_period_it->second > 50){
        return false;
    }

    if(long_period_it->second <= 9 || long_period_it->second > 200){
        return false;
    }

    if(short_period_it->second >= long_period_it->second){
        return false;
    }
    
    return true;
}