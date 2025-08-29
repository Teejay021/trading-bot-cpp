#pragma once

// Main trading bot header that includes all components
#include "data/csv_parser.h"
#include "strategy/strategy.h"
#include "risk/risk_manager.h"
#include "backtester/backtester.h"
#include "reporting/report_generator.h"
#include "utils/logger.h"

namespace TradingBot {
    // Main trading bot class that orchestrates all components
    class TradingBot {
    public:
        TradingBot();
        ~TradingBot();
        
        // Initialize the bot with configuration
        bool initialize(const std::string& config_file);
        
        // Run backtesting
        bool run_backtest(const std::string& data_file, const std::string& strategy_name);
        
        // Generate reports
        void generate_report(const std::string& output_file);
        
        // Get performance metrics
        const BacktestResults& get_results() const;
        
    private:
        std::unique_ptr<CSVParser> csv_parser_;
        std::unique_ptr<Strategy> strategy_;
        std::unique_ptr<RiskManager> risk_manager_;
        std::unique_ptr<Backtester> backtester_;
        std::unique_ptr<ReportGenerator> report_generator_;
        std::unique_ptr<Logger> logger_;
        
        BacktestResults results_;
    };
}
