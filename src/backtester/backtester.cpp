#include "backtester/backtester.h"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>

namespace TradingBot {

Backtester::Backtester() {}

Backtester::~Backtester() {}

bool Backtester::initialize(const BacktestConfig& config) {
    config_ = config;
    
    
    if (config.initial_capital <= 0.0) {
        return false;
    }
    
    if (config.commission_rate < 0.0 || config.commission_rate > 1.0) {
        return false;
    }
    
    if (config.slippage < 0.0 || config.slippage > 1.0) {
        return false;
    }
    
    return true;
}

BacktestResults Backtester::run_backtest(std::shared_ptr<Strategy> strategy,
                                        std::shared_ptr<CSVParser> data_parser,
                                        std::shared_ptr<RiskManager> risk_manager) {
    // Initialize results
    results_ = BacktestResults();
    
    if (!strategy || !data_parser || !risk_manager) {
        throw std::invalid_argument("Null pointer provided to run_backtest");
    }
    
    
    PortfolioState portfolio;
    portfolio.cash = config_.initial_capital;
    portfolio.total_value = config_.initial_capital;
    
    Position current_position;
    
    
    size_t data_count = data_parser->get_data_count();
    
    for (size_t i = 0; i < data_count; ++i) {
        const MarketData& current_data = data_parser->get_data(i);
        
        
        TradingSignal signal;
        try {
            signal = strategy->generate_signal(current_data, current_position);
        } catch (const std::exception& e) {
            
            continue;
        }
        
        
        if (signal.type != SignalType::HOLD && risk_manager->validate_trade(signal, portfolio)) {
            
            
            double position_size = risk_manager->calculate_position_size(signal, portfolio, current_data);
            signal.quantity = position_size;
            
            
            Trade trade;
            execute_trade(trade, signal, current_data, portfolio);
            
            
            risk_manager->update_portfolio_state(portfolio, signal, current_data);
            
            
            if (signal.type == SignalType::BUY) {
                current_position.quantity += signal.quantity;
                current_position.avg_price = signal.price; // Simplified - should be weighted average
                current_position.symbol = "STOCK"; // Simplified - should track actual symbol
            } else if (signal.type == SignalType::SELL) {
                current_position.quantity -= signal.quantity;
                if (current_position.quantity <= 0) {
                    current_position.quantity = 0.0;
                    current_position.avg_price = 0.0;
                }
            }
            
            // Record the trade
            results_.trades.push_back(trade);
        }
        
        // Check for risk-based position closures (stop-loss, take-profit)
        if (current_position.quantity > 0 && 
            risk_manager->should_close_position(current_position, current_data, portfolio)) {
            
            // Create sell signal for position closure
            TradingSignal close_signal;
            close_signal.type = SignalType::SELL;
            close_signal.price = current_data.close;
            close_signal.quantity = current_position.quantity;
            close_signal.timestamp = current_data.timestamp;
            close_signal.reason = "Risk management closure (stop-loss/take-profit)";
            
            // Execute the closure trade
            Trade close_trade;
            execute_trade(close_trade, close_signal, current_data, portfolio);
            risk_manager->update_portfolio_state(portfolio, close_signal, current_data);
            
            // Reset position
            current_position.quantity = 0.0;
            current_position.avg_price = 0.0;
            
            // Record the trade
            results_.trades.push_back(close_trade);
        }
        
        // Update equity curve
        update_equity_curve(portfolio.total_value);
    }
    
    // Calculate final statistics
    calculate_statistics();
    
    return results_;
}

const BacktestConfig& Backtester::get_config() const {
    return config_;
}

void Backtester::set_config(const BacktestConfig& config) {
    config_ = config;
}

const BacktestResults& Backtester::get_results() const {
    return results_;
}

// Private helper methods

void Backtester::execute_trade(Trade& trade, const TradingSignal& signal, 
                              const MarketData& data, PortfolioState& portfolio) {
    // Fill trade details
    trade.timestamp = signal.timestamp;
    trade.price = signal.price;
    trade.quantity = signal.quantity;
    
    // Apply slippage
    if (signal.type == SignalType::BUY) {
        trade.action = "BUY";
        trade.price *= (1.0 + config_.slippage); // Buy at higher price
    } else if (signal.type == SignalType::SELL) {
        trade.action = "SELL";
        trade.price *= (1.0 - config_.slippage); // Sell at lower price
    }
    
    // Calculate commission
    double trade_value = trade.price * trade.quantity;
    trade.commission = trade_value * config_.commission_rate;
    
    // TODO: Calculate P&L for the trade
    // For now, set to 0 - will be calculated in statistics
    trade.pnl = 0.0;
}

void Backtester::update_equity_curve(double current_value) {
    results_.equity_curve.push_back(current_value);
}

void Backtester::calculate_statistics() {
    if (results_.trades.empty()) {
        return; // No trades to analyze
    }
    
    // TODO: Implement comprehensive statistics calculation
    // 1. Calculate total return
    // 2. Calculate win rate
    // 3. Calculate Sharpe ratio
    // 4. Calculate maximum drawdown
    // 5. Calculate profit factor
    
    results_.total_trades = static_cast<int>(results_.trades.size());
    
    // Count winning and losing trades
    for (const auto& trade : results_.trades) {
        if (trade.pnl > 0) {
            results_.winning_trades++;
        } else if (trade.pnl < 0) {
            results_.losing_trades++;
        }
    }
    
    // Calculate win rate
    if (results_.total_trades > 0) {
        results_.win_rate = static_cast<double>(results_.winning_trades) / results_.total_trades;
    }
    
    // Calculate total return
    if (!results_.equity_curve.empty() && config_.initial_capital > 0) {
        double final_value = results_.equity_curve.back();
        results_.total_return = (final_value - config_.initial_capital) / config_.initial_capital;
    }
    
    // Calculate maximum drawdown
    if (!results_.equity_curve.empty()) {
        results_.max_drawdown = calculate_max_drawdown(results_.equity_curve);
    }
    
    // TODO: Calculate additional metrics
    // - Annualized return
    // - Sharpe ratio
    // - Average win/loss
    // - Profit factor
}

double Backtester::calculate_sharpe_ratio(const std::vector<double>& returns) {
    if (returns.size() < 2) {
        return 0.0;
    }
    
    // Calculate mean return
    double mean_return = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    
    // Calculate standard deviation
    double variance = 0.0;
    for (double ret : returns) {
        variance += std::pow(ret - mean_return, 2);
    }
    variance /= (returns.size() - 1);
    double std_dev = std::sqrt(variance);
    
    if (std_dev == 0.0) {
        return 0.0;
    }
    
    // Assume risk-free rate of 0 for simplicity
    return mean_return / std_dev;
}

double Backtester::calculate_max_drawdown(const std::vector<double>& equity_curve) {
    if (equity_curve.empty()) {
        return 0.0;
    }
    
    double max_drawdown = 0.0;
    double peak = equity_curve[0];
    
    for (double value : equity_curve) {
        if (value > peak) {
            peak = value;
        }
        
        double drawdown = (peak - value) / peak;
        if (drawdown > max_drawdown) {
            max_drawdown = drawdown;
        }
    }
    
    return max_drawdown;
}

} // namespace TradingBot
