#include "strategy/strategy.h"
#include <map>
#include <string>

namespace TradingBot {

SMACrossoverStrategy::SMACrossoverStrategy() : Strategy("SMA_CROSSOVER") {
}

bool SMACrossoverStrategy::initialize(const std::map<std::string, double>& params) {

    auto short_it = params.find("short_period");
    auto long_it = params.find("long_period");

    if (short_it == params.end() || long_it == params.end()) {
        throw std::invalid_argument("Missing required parameters");
    }

    short_period_ = static_cast<int>(short_it->second);
    long_period_ = static_cast<int>(long_it->second);    
    
    return true;
}

TradingSignal SMACrossoverStrategy::generate_signal(const MarketData& data, const Position& current_position) {
    TradingSignal signal;
    signal.timestamp = data.timestamp;
    signal.type = SignalType::HOLD;
    
    // Add current data to price history
    price_history_.push_back(data);
    
    // Keep only what we need to avoid memory bloat
    if (price_history_.size() > static_cast<size_t>(long_period_ + 1)) {
        price_history_.erase(price_history_.begin());
    }
    
    // Need enough data for both SMAs
    if (price_history_.size() < static_cast<size_t>(long_period_)) {
        return signal; // Not enough data, return HOLD
    }
    
    
    double short_sma = calculate_sma(price_history_, short_period_);
    double long_sma = calculate_sma(price_history_, long_period_);
    
    // Calculate previous SMAs for crossover detection
    if (price_history_.size() > static_cast<size_t>(long_period_)) {
        // Previous data (without the last element)
        std::vector<MarketData> prev_data(price_history_.begin(), price_history_.end() - 1);
        
        double prev_short_sma = calculate_sma(prev_data, short_period_);
        double prev_long_sma = calculate_sma(prev_data, long_period_);
        
        // Detect crossovers
        if (prev_short_sma <= prev_long_sma && short_sma > long_sma) {
            // Short SMA crossed ABOVE long SMA → BUY
            signal.type = SignalType::BUY;
            signal.price = data.close;
            signal.quantity = 100.0; // Will be adjusted by risk management
            signal.reason = "Short SMA crossed above long SMA";
        } else if (prev_short_sma >= prev_long_sma && short_sma < long_sma) {
            // Short SMA crossed BELOW long SMA → SELL
            signal.type = SignalType::SELL;
            signal.price = data.close;
            signal.quantity = current_position.quantity;
            signal.reason = "Short SMA crossed below long SMA";
        }
    }
    
    return signal;
}

std::map<std::string, double> SMACrossoverStrategy::get_parameters() const {
    return std::map<std::string, double>{
        {"short_period", static_cast<double>(short_period_)}, 
        {"long_period", static_cast<double>(long_period_)}
    };
}

bool SMACrossoverStrategy::validate_parameters(const std::map<std::string, double>& params) const {
    
    auto short_it = params.find("short_period");
    auto long_it = params.find("long_period");
    
    if (short_it == params.end() || long_it == params.end()) {
        return false;
    }
    
    double short_period = short_it->second;
    double long_period = long_it->second;

    
    if (short_period <= 0 || long_period <= 0) {
        return false;
    }
    
    if (short_period >= long_period) {
        return false;
    }

    return true;
}

} // namespace TradingBot
