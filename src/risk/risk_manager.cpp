#include "risk/risk_manager.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace TradingBot {

RiskManager::RiskManager() {
    // risk_params_ is automatically initialized with default values from RiskParameters constructor
}

RiskManager::~RiskManager() {
    // No cleanup needed. All members are automatically destroyed
}

bool RiskManager::initialize(const RiskParameters& params) {
    risk_params_ = params;
    
    // Validate risk parameters
    if (params.max_position_size <= 0.0 || params.max_position_size > 1.0) {
        return false;
    }
    
    if (params.max_drawdown <= 0.0 || params.max_drawdown > 1.0) {
        return false;
    }
    
    if (params.stop_loss_pct <= 0.0 || params.stop_loss_pct > 1.0) {
        return false;
    }
    
    if (params.take_profit_pct <= 0.0 || params.take_profit_pct > 1.0) {
        return false;
    }
    
    return true;
}

bool RiskManager::validate_trade(const TradingSignal& signal, const PortfolioState& portfolio) {
    
    if (!check_drawdown_limit(portfolio)) {
        return false;
    }
    
    
    if (!check_daily_loss_limit(portfolio)) {
        return false;
    }
    
   
    if (signal.type == SignalType::HOLD) {
        return false;
    }
    
    
    if (signal.price <= 0.0) {
        return false;
    }
    
    return true;
}

double RiskManager::calculate_position_size(const TradingSignal& signal, 
                                          const PortfolioState& portfolio,
                                          const MarketData& current_data) {
    // Calculate position size based on risk management rules
    
    if (signal.type == SignalType::HOLD) {
        return 0.0;
    }
    
    
    double max_risk_amount = portfolio.total_value * risk_params_.max_position_size;
    
    // Simple position sizing without ATR (since we only have current_data)
    // TODO: Modify function signature to accept historical data for ATR calculation
    double position_size = max_risk_amount / signal.price;
    
    // Max 10% of portfolio in one stock
    double max_shares_by_portfolio = portfolio.total_value * 0.1 / signal.price;
    position_size = std::min(position_size, max_shares_by_portfolio);
    
    return position_size;
}

void RiskManager::update_portfolio_state(PortfolioState& portfolio, 
                                        const TradingSignal& signal,
                                        const MarketData& data) {
    
    
    if (signal.type == SignalType::HOLD) {
        return;
    }
    
    
    if (signal.type == SignalType::BUY) {
        // Buying: Decrease cash, increase position value
        portfolio.cash -= signal.price * signal.quantity;
        
        // In a full implementation, individual positions are going to be tracked here!

        
    } else if (signal.type == SignalType::SELL) {
        
        portfolio.cash += signal.price * signal.quantity;
        
        // Update realized P&L (simplified - assumes we're selling at current price)
        // In full implementation: (sell_price - buy_price) * quantity
        // For now, we'll track it in the portfolio total value
    }
    
    // total portfolio value (cash + position values)
    // Note: This is simplified - in full implementation we'd track all open positions
    double previous_total_value = portfolio.total_value;
    
    // For now, assume total_value = cash (since we're not tracking individual positions)
    // In full backtester, this would include current market value of all holdings
    portfolio.total_value = portfolio.cash;
    

    static double peak_value = portfolio.total_value;
    
    if (portfolio.total_value > peak_value) {
        peak_value = portfolio.total_value;
        portfolio.current_drawdown = 0.0; // New high, reset drawdown
    } else {
        portfolio.current_drawdown = calculate_drawdown(peak_value, portfolio.total_value);
        
        // Update max drawdown if current is worse
        if (portfolio.current_drawdown > portfolio.max_drawdown) {
            portfolio.max_drawdown = portfolio.current_drawdown;
        }
    }
    
    //Update unrealized P&L (simplified calculation)
    portfolio.unrealized_pnl = portfolio.total_value - 100000.0; // Assuming $100k initial capital
    
    // Note: For a complete implementation, We need to:
    // - Track individual positions with entry prices
    // - Calculate daily P&L changes
    // - Update position-specific unrealized P&L
    // - Handle partial position closures
    // - Track commission costs
    
}

bool RiskManager::should_close_position(const Position& position, 
                                       const MarketData& current_data,
                                       const PortfolioState& portfolio) {
    // Check if position should be closed due to stop-loss or take-profit
    
    if (position.quantity == 0.0) {
        return false;
    }
    
    // TODO: Implement stop-loss and take-profit logic
    // 1. Calculate current P&L
    // 2. Check stop-loss threshold
    // 3. Check take-profit threshold
    // 4. Check time-based exits if needed
    
    double current_price = current_data.close;
    double entry_price = position.avg_price;
    
    
    double pct_change = (current_price - entry_price) / entry_price;
    
    
    if (position.quantity > 0 && pct_change <= -risk_params_.stop_loss_pct) {
        return true;
    }
    
    
    if (position.quantity > 0 && pct_change >= risk_params_.take_profit_pct) {
        return true;
    }
    
    // TODO: Add logic for short positions
    
    return false;
}

const RiskParameters& RiskManager::get_risk_parameters() const {
    return risk_params_;
}

void RiskManager::set_risk_parameters(const RiskParameters& params) {
    risk_params_ = params;
}

double RiskManager::calculate_atr(const std::vector<MarketData>& data, int period) {
    // Calculate Average True Range for volatility measurement
    
    if (data.size() < static_cast<size_t>(period + 1)) {
        throw std::invalid_argument("Not enough data to calculate ATR");
    }
    
    // TODO: Implement ATR calculation
    // 1. Calculate True Range for each period
    // 2. Calculate Simple Moving Average of True Ranges
    
    std::vector<double> true_ranges;
    
    for (size_t i = 1; i < data.size(); ++i) {
        double high_low = data[i].high - data[i].low;
        double high_close_prev = std::abs(data[i].high - data[i-1].close);
        double low_close_prev = std::abs(data[i].low - data[i-1].close);
        
        double true_range = std::max({high_low, high_close_prev, low_close_prev});
        true_ranges.push_back(true_range);
    }
    
    // Calculate average of last 'period' true ranges
    if (true_ranges.size() < static_cast<size_t>(period)) {
        throw std::invalid_argument("Not enough true range data");
    }
    
    double sum = 0.0;
    for (int i = true_ranges.size() - period; i < static_cast<int>(true_ranges.size()); ++i) {
        sum += true_ranges[i];
    }
    
    return sum / period;
}

double RiskManager::calculate_drawdown(double peak_value, double current_value) {
    // Calculate drawdown percentage
    
    if (peak_value <= 0.0) {
        return 0.0;
    }
    
    if (current_value >= peak_value) {
        return 0.0; // No drawdown
    }
    
    return (peak_value - current_value) / peak_value;
}


// Private helper methods

bool RiskManager::check_drawdown_limit(const PortfolioState& portfolio) {
    // Check if current drawdown exceeds maximum allowed
    
    return portfolio.current_drawdown <= risk_params_.max_drawdown;
}

bool RiskManager::check_daily_loss_limit(const PortfolioState& portfolio) {
    // Check if daily loss exceeds maximum allowed
    

    return true;
}

double RiskManager::calculate_kelly_criterion(double win_rate, double avg_win, double avg_loss) {
    // Calculate Kelly Criterion for optimal position sizing
    
    if (avg_loss <= 0.0) {
        return 0.0;
    }
    
    double win_loss_ratio = avg_win / avg_loss;
    double kelly_fraction = win_rate - ((1.0 - win_rate) / win_loss_ratio);
    
    // Cap Kelly fraction to reasonable limits (e.g., 25%)
    return std::min(kelly_fraction, 0.25);
}

} // namespace TradingBot
