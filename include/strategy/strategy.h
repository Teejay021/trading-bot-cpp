#pragma once

#include "data/csv_parser.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace TradingBot {

    // Trading signal types
    enum class SignalType {
        BUY,
        SELL,
        HOLD
    };

    // Trading signal structure
    struct TradingSignal {
        SignalType type;
        double price;
        double quantity;
        std::string timestamp;
        std::string reason;
        
        TradingSignal() : type(SignalType::HOLD), price(0.0), quantity(0.0) {}
    };

    // Position structure
    struct Position {
        double quantity;
        double avg_price;
        std::string symbol;
        
        Position() : quantity(0.0), avg_price(0.0) {}
    };

    // Base strategy class - all strategies inherit from this
    class Strategy {
    public:
        Strategy(const std::string& name);
        virtual ~Strategy();
        
        // Get strategy name
        const std::string& get_name() const;
        
        // Initialize strategy with parameters
        virtual bool initialize(const std::map<std::string, double>& params) = 0;
        
        // Generate trading signal based on market data
        virtual TradingSignal generate_signal(const MarketData& data, const Position& current_position) = 0;
        
        // Update strategy state
        virtual void update(const MarketData& data);
        
        // Get strategy parameters
        virtual std::map<std::string, double> get_parameters() const = 0;
        
        // Validate strategy parameters
        virtual bool validate_parameters(const std::map<std::string, double>& params) const = 0;
        
    protected:
        std::string name_;
        std::map<std::string, double> parameters_;
        
        // Helper methods for common calculations
        double calculate_sma(const std::vector<MarketData>& data, int period);
        double calculate_ema(const std::vector<MarketData>& data, int period);
        double calculate_rsi(const std::vector<MarketData>& data, int period);
    };

    // Simple moving average crossover strategy
    class SMACrossoverStrategy : public Strategy {
    public:
        SMACrossoverStrategy();
        
        bool initialize(const std::map<std::string, double>& params) override;
        TradingSignal generate_signal(const MarketData& data, const Position& current_position) override;
        std::map<std::string, double> get_parameters() const override;
        bool validate_parameters(const std::map<std::string, double>& params) const override;
        
    private:
        int short_period_;
        int long_period_;
        std::vector<MarketData> price_history_;
    };

    // RSI strategy for overbought/oversold conditions
    class RSIStrategy : public Strategy {
    public:
        RSIStrategy();
        
        bool initialize(const std::map<std::string, double>& params) override;
        TradingSignal generate_signal(const MarketData& data, const Position& current_position) override;
        std::map<std::string, double> get_parameters() const override;
        bool validate_parameters(const std::map<std::string, double>& params) const override;
        
    private:
        int rsi_period_;
        double oversold_threshold_;
        double overbought_threshold_;
        std::vector<MarketData> price_history_;
    };

} // namespace TradingBot
