#include "strategy/strategy.h"
#include <cmath>
#include <algorithm>

namespace TradingBot {

Strategy::Strategy(const std::string& name) : name_(name) {
}

Strategy::~Strategy() {
}

void Strategy::update(const MarketData& data) {
    
}

const std::string& Strategy::get_name() const {
    return name_;
}

double Strategy::calculate_sma(const std::vector<MarketData>& data, int period) {

    if(data.size() < period){
        throw std::invalid_argument("Data size is less than period");
    }

    double SMA = 0.0;
   
    // Use the last 'period' elements
    for(size_t i = data.size() - period; i < data.size(); i++){
        SMA += data[i].close;
    }
    SMA /= period;  // Divide by period, not data.size()
    return SMA;

}

double Strategy::calculate_ema(const std::vector<MarketData>& data, int period) {
   
    if(data.size() < period){
        throw std::invalid_argument("Data size is less than period");
    }

    double multiplier = 2.0 / (period + 1);
    
    //SMA of first 'period' elements
    double EMA = 0.0;
    for(int i = 0; i < period; i++){
        EMA += data[i].close;
    }
    EMA /= period;

    //EMA for remaining elements
    for(size_t i = period; i < data.size(); i++){
        EMA = (data[i].close * multiplier) + (EMA * (1 - multiplier));
    }
    return EMA;

}

double Strategy::calculate_rsi(const std::vector<MarketData>& data, int period) {
    
    if(data.size() < period + 1){
        throw std::invalid_argument("Data size is less than period + 1");
    }

    // Calculate price changes (gains and losses)
    std::vector<double> gains, losses;
    for (size_t i = 1; i < data.size(); i++) {
        double change = data[i].close - data[i-1].close;
        if (change > 0) {
            gains.push_back(change);
            losses.push_back(0.0);
        } else {
            gains.push_back(0.0);
            losses.push_back(-change);  // Make loss positive
        }
    }
    
    // Calculate average gains and losses
    double avg_gain = 0.0, avg_loss = 0.0;
    for (size_t i = gains.size() - period; i < gains.size(); i++) {
        avg_gain += gains[i];
        avg_loss += losses[i];
    }
    avg_gain /= period;
    avg_loss /= period;
    
    // Calculate RSI
    if (avg_loss == 0.0) {
        return 100.0;  // All gains, no losses
    } else {
        double RS = avg_gain / avg_loss;
        return 100.0 - (100.0 / (1.0 + RS));
    }
}

} // namespace TradingBot
