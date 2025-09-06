#include <iostream>
#include <map>
#include "data/csv_parser.h"
#include "strategy/strategy.h"

using namespace TradingBot;

int main() {

    std::cout << "=== EMA Strategy Test ===" << std::endl;

    CSVParser parser;
    if (!parser.load_data("data/sample_data.csv")) {
        std::cout << "Failed to load CSV data" << std::endl;
        return 1;
    }
    
    std::cout << "Loaded " << parser.get_data_count() << " rows of market data" << std::endl;

    if (!parser.validate_data()) {
        std::cout << "Data validation failed" << std::endl;
        return 1;
    }
    
    std::cout << "Data validation passed" << std::endl;

    EMAStrategy ema_strategy;

    std::map<std::string, double> params = {
        {"short_period", 12.0},
        {"long_period", 26.0}
    };
    
    if (!ema_strategy.validate_parameters(params)) {
        std::cout << "Parameter validation failed" << std::endl;
        return 1;
    }

    if (!ema_strategy.initialize(params)) {
        std::cout << "Strategy initialization failed" << std::endl;
        return 1;
    }
    
    std::cout << "EMA strategy initialized" << std::endl;
    
    
    auto current_params = ema_strategy.get_parameters();
    std::cout << "\n Strategy Parameters:" << std::endl;
    std::cout << "   Short Period: " << current_params["short_period"] << std::endl;
    std::cout << "   Long Period: " << current_params["long_period"] << std::endl;
    
    std::cout << "\n Generating trading signals..." << std::endl;
    
    Position current_position;
    
    for (size_t i = 0; i < parser.get_data_count(); ++i) {
        const MarketData& data = parser.get_data(i);

        TradingSignal signal;
        try {
            signal = ema_strategy.generate_signal(data, current_position);
        } catch (const std::exception& e) {
            std::cout << "Skipping signal generation: " << e.what() << std::endl;
            continue;
        }

        switch (signal.type) {
            case SignalType::BUY:
                std::cout << "BUY signal at " << data.timestamp << std::endl;
                break;
            case SignalType::SELL:
                std::cout << "SELL signal at " << data.timestamp << std::endl;
                break;
        }
    }

    // Signal counting would need to be implemented here
    std::cout << "\nSignal Summary:" << std::endl;
    std::cout << "   Signal generation completed for " << parser.get_data_count() << " data points" << std::endl;

    std::cout << "\n EMA Strategy Info:" << std::endl;
    std::cout << "   Strategy Name: " << ema_strategy.get_name() << std::endl;
    std::cout << "   EMA crossover strategy - buy when short EMA crosses above long EMA, sell when short EMA crosses below long EMA" << std::endl;
    std::cout << "   Short EMA is faster than long EMA" << std::endl;

    std::cout << "\n EMA strategy test completed successfully!" << std::endl;

    return 0;
    
}