#include <iostream>
#include "risk/risk_manager.h"

using namespace TradingBot;

int main() {
    std::cout << "=== Risk Manager Test ===" << std::endl;
    

    RiskManager risk_manager;
    
 
    RiskParameters params;
    params.max_position_size = 0.02;
    params.stop_loss_pct = 0.05;    
    
    if (risk_manager.initialize(params)) {
        std::cout << "Risk Manager initialized successfully" << std::endl;
    } else {
        std::cout << "Risk Manager initialization failed" << std::endl;
        return 1;
    }
    
    // Test position sizing
    PortfolioState portfolio;
    portfolio.cash = 100000.0;
    portfolio.total_value = 100000.0;
    
    TradingSignal signal;
    signal.type = SignalType::BUY;
    signal.price = 50.0;
    signal.quantity = 0.0; // Will be calculated
    
    MarketData data;
    data.close = 50.0;
    
    if (risk_manager.validate_trade(signal, portfolio)) {
        double position_size = risk_manager.calculate_position_size(signal, portfolio, data);
        std::cout << "Position size calculated: " << position_size << " shares" << std::endl;
        std::cout << "Total cost: $" << (position_size * signal.price) << std::endl;
        
        signal.quantity = position_size;
        
        // Test portfolio update
        risk_manager.update_portfolio_state(portfolio, signal, data);
        std::cout << "Portfolio updated - Cash: $" << portfolio.cash << std::endl;
        
    } else {
        std::cout << "Trade validation failed" << std::endl;
    }
    
    std::cout << "Risk Manager test completed!" << std::endl;
    return 0;
}
