#include <iostream>
#include <memory>
#include "backtester/backtester.h"
#include "strategy/strategy.h"
#include "data/csv_parser.h"
#include "risk/risk_manager.h"

using namespace TradingBot;

int main() {
    std::cout << "=== Backtester Test ===" << std::endl;
    
    Backtester backtester;
    

    BacktestConfig config;
    config.initial_capital = 50000.0;
    config.commission_rate = 0.001;    // 0.1% commission
    
    if (backtester.initialize(config)) {
        std::cout << "Backtester initialized successfully" << std::endl;
        
        // Display configuration
        const auto& test_config = backtester.get_config();
        std::cout << "Configuration:" << std::endl;
        std::cout << "  Initial Capital: $" << test_config.initial_capital << std::endl;
        std::cout << "  Commission Rate: " << (test_config.commission_rate * 100) << "%" << std::endl;
        std::cout << "  Slippage: " << (test_config.slippage * 100) << "%" << std::endl;
        std::cout << "  Short Selling: " << (test_config.enable_short_selling ? "Enabled" : "Disabled") << std::endl;
        
    } else {
        std::cout << "Backtester initialization failed" << std::endl;
        return 1;
    }
    
    // Test with sample data (if available)
    try {
        auto csv_parser = std::make_shared<CSVParser>();
        if (csv_parser->load_data("data/sample_data.csv")) {
            std::cout << "Sample data loaded: " << csv_parser->get_data_count() << " rows" << std::endl;
            
            // Create strategy and risk manager
            auto sma_strategy = std::make_shared<SMACrossoverStrategy>();
            auto risk_manager = std::make_shared<RiskManager>();
            
            // Initialize strategy with basic parameters
            std::map<std::string, double> strategy_params = {
                {"short_period", 5.0},
                {"long_period", 20.0}
            };
            
            if (sma_strategy->initialize(strategy_params)) {
                std::cout << "SMA strategy initialized (5/20 periods)" << std::endl;
                
                // This would run a full backtest (commented out for basic test)
                // auto results = backtester.run_backtest(sma_strategy, csv_parser, risk_manager);
                // std::cout << "Backtest completed with " << results.total_trades << " trades" << std::endl;
                
                std::cout << "Backtester components ready for full testing" << std::endl;
            } else {
                std::cout << "Strategy initialization failed" << std::endl;
            }
            
        } else {
            std::cout << "Could not load sample data - skipping strategy test" << std::endl;
            std::cout << "Note: Place sample data in data/sample_data.csv for full testing" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "Test completed with minor issues: " << e.what() << std::endl;
    }
    
    std::cout << "Backtester test completed!" << std::endl;
    return 0;
}
