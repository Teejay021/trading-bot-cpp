#pragma once

#include "strategy/strategy.h"
#include <string>
#include <vector>

namespace TradingBot {

    // Risk management parameters
    struct RiskParameters {
        double max_position_size;      // Maximum position size as % of portfolio
        double max_drawdown;           // Maximum allowed drawdown
        double stop_loss_pct;          // Stop loss percentage
        double take_profit_pct;        // Take profit percentage
        double max_daily_loss;         // Maximum daily loss
        double position_sizing_atr;    // Position sizing based on ATR
        
        RiskParameters() : 
            max_position_size(0.02),   // 2% max position size
            max_drawdown(0.20),        // 20% max drawdown
            stop_loss_pct(0.05),       // 5% stop loss
            take_profit_pct(0.10),     // 10% take profit
            max_daily_loss(0.05),      // 5% max daily loss
            position_sizing_atr(2.0)   // 2x ATR for position sizing
        {}
    };

    // Portfolio state
    struct PortfolioState {
        double cash;
        double total_value;
        double unrealized_pnl;
        double realized_pnl;
        double max_drawdown;
        double current_drawdown;
        
        PortfolioState() : 
            cash(100000.0),           // Start with $100k
            total_value(100000.0),
            unrealized_pnl(0.0),
            realized_pnl(0.0),
            max_drawdown(0.0),
            current_drawdown(0.0)
        {}
    };

    // Risk Manager class
    class RiskManager {
    public:
        RiskManager();
        ~RiskManager();
        
        // Initialize with risk parameters
        bool initialize(const RiskParameters& params);
        
        // Check if trade is allowed
        bool validate_trade(const TradingSignal& signal, const PortfolioState& portfolio);
        
        // Calculate position size based on risk
        double calculate_position_size(const TradingSignal& signal, 
                                    const PortfolioState& portfolio,
                                    const MarketData& current_data);
        
        // Update portfolio state
        void update_portfolio_state(PortfolioState& portfolio, 
                                  const TradingSignal& signal,
                                  const MarketData& data);
        
        // Check stop loss and take profit
        bool should_close_position(const Position& position, 
                                 const MarketData& current_data,
                                 const PortfolioState& portfolio);
        
        // Get risk parameters
        const RiskParameters& get_risk_parameters() const;
        
        // Set risk parameters
        void set_risk_parameters(const RiskParameters& params);
        
        // Calculate ATR (Average True Range)
        double calculate_atr(const std::vector<MarketData>& data, int period);
        
        // Calculate drawdown
        double calculate_drawdown(double peak_value, double current_value);
        
    private:
        RiskParameters risk_params_;
        
        // Helper methods
        bool check_drawdown_limit(const PortfolioState& portfolio);
        bool check_daily_loss_limit(const PortfolioState& portfolio);
        double calculate_kelly_criterion(double win_rate, double avg_win, double avg_loss);
    };

} // namespace TradingBot
