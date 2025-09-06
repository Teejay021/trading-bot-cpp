#include "strategy/strategy.h"
#include "data/csv_parser.h"
#include <iostream>
#include <map>

int main() {
    std::cout << "Testing SMA Crossover Strategy..." << std::endl;
    
    // Load sample data
    TradingBot::CSVParser parser;
    if (!parser.load_data("data/sample_data.csv")) {
        std::cout << " Failed to load CSV data" << std::endl;
        return 1;
    }
    std::cout << " Loaded " << parser.get_data_count() << " rows of data" << std::endl;
    
    TradingBot::SMACrossoverStrategy strategy;
    

    std::map<std::string, double> params = {
        {"short_period", 2.0},   // 5-day SMA (short-term)
        {"long_period", 10.0}    // 10-day SMA (long-term)
    };
    
    if (!strategy.initialize(params)) {
        std::cout << " Failed to initialize strategy" << std::endl;
        return 1;
    }
    std::cout << " Strategy initialized with 5-day and 10-day SMAs" << std::endl;
    
    // Test parameter validation
    if (!strategy.validate_parameters(params)) {
        std::cout << " Parameter validation failed" << std::endl;
        return 1;
    }
    std::cout << " Parameter validation passed" << std::endl;
    
    // Simulate trading signals
    TradingBot::Position position; // Start with empty position
    int buy_signals = 0, sell_signals = 0, hold_signals = 0;
    
    std::cout << "\n=== Trading Signals ===" << std::endl;
    
    for (size_t i = 0; i < parser.get_data_count(); i++) {
        const auto& data = parser.get_data(i);
        
        // Generate signal
        TradingBot::TradingSignal signal = strategy.generate_signal(data, position);
        
        // Count signal types
        switch (signal.type) {
            case TradingBot::SignalType::BUY:
                buy_signals++;
                std::cout << "BUY  @ " << signal.timestamp 
                          << " | Price: $" << signal.price 
                          << " | Qty: " << signal.quantity 
                          << " | " << signal.reason << std::endl;
                // Update position simplified version
                position.quantity += signal.quantity;
                position.avg_price = signal.price;
                break;
                
            case TradingBot::SignalType::SELL:
                sell_signals++;
                std::cout << "SELL @ " << signal.timestamp 
                          << " | Price: $" << signal.price 
                          << " | Qty: " << signal.quantity 
                          << " | " << signal.reason << std::endl;
                // Update position simplified version
                position.quantity = 0;
                position.avg_price = 0;
                break;
                
            case TradingBot::SignalType::HOLD:
                hold_signals++;
                break;
        }
    }
    
    // Display results
    std::cout << "\n=== Strategy Test Results ===" << std::endl;

    std::cout << "Total Data Points: " << parser.get_data_count() << std::endl;

    std::cout << "BUY Signals: " << buy_signals << std::endl;
    std::cout << "SELL Signals: " << sell_signals << std::endl;
    std::cout << "HOLD Signals: " << hold_signals << std::endl;
    
    // Test parameter retrieval
    auto retrieved_params = strategy.get_parameters();
    std::cout << "\nRetrieved Parameters:" << std::endl;
    for (const auto& param : retrieved_params) {
        std::cout << "  " << param.first << ": " << param.second << std::endl;
    }
    
    std::cout << "\n SMA Crossover Strategy test completed!" << std::endl;
    return 0;
}
