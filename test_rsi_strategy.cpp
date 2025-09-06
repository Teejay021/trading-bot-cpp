#include <iostream>
#include <map>
#include "data/csv_parser.h"
#include "strategy/strategy.h"

using namespace TradingBot;

int main() {
    std::cout << "=== RSI Strategy Test ===" << std::endl;
    
    // 1. Load CSV data
    CSVParser parser;
    if (!parser.load_data("data/sample_data.csv")) {
        std::cout << "Failed to load CSV data" << std::endl;
        return 1;
    }
    
    std::cout << "Loaded " << parser.get_data_count() << " rows of market data" << std::endl;
    
    // 2. Validate data
    if (!parser.validate_data()) {
        std::cout << "Data validation failed" << std::endl;
        return 1;
    }
    
    std::cout << "Data validation passed" << std::endl;
    
    // 3. Create RSI strategy
    RSIStrategy rsi_strategy;
    
    // 4. Set up strategy parameters
    std::map<std::string, double> params = {
        {"period", 14.0},              // 14-day RSI (standard)
        {"overbought_threshold", 70.0}, // Sell when RSI > 70
        {"oversold_threshold", 30.0}    // Buy when RSI < 30
    };
    
    // 5. Validate parameters
    if (!rsi_strategy.validate_parameters(params)) {
        std::cout << "Parameter validation failed" << std::endl;
        return 1;
    }
    
    std::cout << "Parameters validated successfully" << std::endl;
    
    // 6. Initialize strategy
    if (!rsi_strategy.initialize(params)) {
        std::cout << "Strategy initialization failed" << std::endl;
        return 1;
    }
    
    std::cout << "RSI strategy initialized" << std::endl;
    
    // 7. Display strategy parameters
    auto current_params = rsi_strategy.get_parameters();
    std::cout << "\n Strategy Parameters:" << std::endl;
    std::cout << "   RSI Period: " << current_params["period"] << " days" << std::endl;
    std::cout << "   Overbought Threshold: " << current_params["overbought_threshold"] << std::endl;
    std::cout << "   Oversold Threshold: " << current_params["oversold_threshold"] << std::endl;
    
    // 8. Simulate trading signals
    std::cout << "\n Generating trading signals..." << std::endl;
    
    Position current_position;
    current_position.quantity = 0.0;
    current_position.avg_price = 0.0;
    
    int buy_signals = 0;
    int sell_signals = 0;
    int hold_signals = 0;
    
    // Process each data point
    for (size_t i = 0; i < parser.get_data_count(); ++i) {
        const MarketData& data = parser.get_data(i);
        
        TradingSignal signal;
        try {
            signal = rsi_strategy.generate_signal(data, current_position);
        } catch (const std::exception& e) {
            // Skip if not enough data for RSI calculation yet
            std::cout << "Skipping signal generation: " << e.what() << std::endl;
            continue;
        }
        
        // Count signal types
        switch (signal.type) {
            case SignalType::BUY:
                buy_signals++;
                std::cout << "BUY signal at " << data.timestamp 
                         << " - Price: $" << data.close 
                         << " - Reason: " << signal.reason << std::endl;
                // Update position for testing
                current_position.quantity += signal.quantity;
                current_position.avg_price = signal.price;
                break;
                
            case SignalType::SELL:
                sell_signals++;
                std::cout << "SELL signal at " << data.timestamp 
                         << " - Price: $" << data.close 
                         << " - Reason: " << signal.reason << std::endl;
                // Update position for testing
                current_position.quantity = 0.0;
                current_position.avg_price = 0.0;
                break;
                
            case SignalType::HOLD:
                hold_signals++;
                break;
        }
    }
    
    
    std::cout << "\n Signal Summary:" << std::endl;
    std::cout << "   BUY signals: " << buy_signals << std::endl;
    std::cout << "   SELL signals: " << sell_signals << std::endl;
    std::cout << "   HOLD signals: " << hold_signals << std::endl;
    std::cout << "   Total signals: " << (buy_signals + sell_signals + hold_signals) << std::endl;
    
    
    std::cout << "\n RSI Strategy Info:" << std::endl;
    std::cout << "   Strategy Name: " << rsi_strategy.get_name() << std::endl;
    std::cout << "   RSI measures momentum - values above 70 indicate overbought conditions" << std::endl;
    std::cout << "   RSI values below 30 indicate oversold conditions" << std::endl;
    
    std::cout << "\n RSI strategy test completed successfully!" << std::endl;
    
    return 0;
}
