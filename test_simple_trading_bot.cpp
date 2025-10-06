#include <iostream>
#include "trading_bot.h"

int main() {
    std::cout << "=== Simple TradingBot Test ===" << std::endl;
    
    try {
        // Test 1: Construction
        TradingBot::TradingBot bot;
        std::cout << "1. TradingBot constructed successfully" << std::endl;
        
        // Test 2: Initialize with config file
        if (bot.initialize("config.json")) {
            std::cout << "2. TradingBot initialized successfully" << std::endl;
        } else {
            std::cout << "2. TradingBot initialization failed (expected - no config file)" << std::endl;
        }
        
        // Test 3: Try to run a backtest
        if (bot.run_backtest("test_data.csv", "SMA_CROSSOVER")) {
            std::cout << "3. Backtest completed successfully" << std::endl;
            
            // Get results
            const auto& results = bot.get_results();
            std::cout << "   Total trades: " << results.total_trades << std::endl;
            std::cout << "   Total return: " << (results.total_return * 100) << "%" << std::endl;
            
        } else {
            std::cout << "3. Backtest failed" << std::endl;
        }
        
        std::cout << "\nAll tests completed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
