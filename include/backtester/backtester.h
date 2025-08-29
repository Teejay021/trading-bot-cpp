#pragma once

#include "data/csv_parser.h"
#include "strategy/strategy.h"
#include "risk/risk_manager.h"
#include <string>
#include <vector>
#include <memory>

namespace TradingBot {

    // Trade record
    struct Trade {
        std::string timestamp;
        std::string action;           // BUY, SELL
        double price;
        double quantity;
        double commission;
        double pnl;
        
        Trade() : price(0.0), quantity(0.0), commission(0.0), pnl(0.0) {}
    };

    // Backtest results
    struct BacktestResults {
        double total_return;
        double annualized_return;
        double sharpe_ratio;
        double max_drawdown;
        double win_rate;
        int total_trades;
        int winning_trades;
        int losing_trades;
        double avg_win;
        double avg_loss;
        double profit_factor;
        std::vector<Trade> trades;
        std::vector<double> equity_curve;
        
        BacktestResults() : 
            total_return(0.0), annualized_return(0.0), sharpe_ratio(0.0),
            max_drawdown(0.0), win_rate(0.0), total_trades(0),
            winning_trades(0), losing_trades(0), avg_win(0.0),
            avg_loss(0.0), profit_factor(0.0)
        {}
    };

    // Backtesting configuration
    struct BacktestConfig {
        double initial_capital;
        double commission_rate;
        double slippage;
        std::string start_date;
        std::string end_date;
        bool enable_short_selling;
        
        BacktestConfig() : 
            initial_capital(100000.0), commission_rate(0.001), slippage(0.0001),
            enable_short_selling(false)
        {}
    };

    // Main backtester class
    class Backtester {
    public:
        Backtester();
        ~Backtester();
        
        // Initialize backtester
        bool initialize(const BacktestConfig& config);
        
        // Run backtest with given strategy and data
        BacktestResults run_backtest(std::shared_ptr<Strategy> strategy,
                                   std::shared_ptr<CSVParser> data_parser,
                                   std::shared_ptr<RiskManager> risk_manager);
        
        // Get backtest configuration
        const BacktestConfig& get_config() const;
        
        // Set backtest configuration
        void set_config(const BacktestConfig& config);
        
        // Get latest results
        const BacktestResults& get_results() const;
        
    private:
        BacktestConfig config_;
        BacktestResults results_;
        
        // Internal methods
        void execute_trade(Trade& trade, const TradingSignal& signal, 
                          const MarketData& data, PortfolioState& portfolio);
        void update_equity_curve(double current_value);
        void calculate_statistics();
        double calculate_sharpe_ratio(const std::vector<double>& returns);
        double calculate_max_drawdown(const std::vector<double>& equity_curve);
    };

} // namespace TradingBot
