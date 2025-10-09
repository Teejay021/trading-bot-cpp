#include <iostream>
#include <iomanip>
#include "trading_bot.h"

using TradingBot::BacktestResults;

void print_separator() {
    std::cout << std::string(80, '=') << std::endl;
}

void print_results(const BacktestResults& results) {
    std::cout << "\n" << std::string(80, '-') << std::endl;
    std::cout << "BACKTEST RESULTS" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    
    std::cout << std::left << std::setw(30) << "Total Return:"
              << std::right << std::fixed << std::setprecision(2)
              << (results.total_return * 100) << "%" << std::endl;
              
    std::cout << std::left << std::setw(30) << "Annualized Return:"
              << std::right << std::fixed << std::setprecision(2)
              << (results.annualized_return * 100) << "%" << std::endl;
              
    std::cout << std::left << std::setw(30) << "Sharpe Ratio:"
              << std::right << std::fixed << std::setprecision(3)
              << results.sharpe_ratio << std::endl;
              
    std::cout << std::left << std::setw(30) << "Max Drawdown:"
              << std::right << std::fixed << std::setprecision(2)
              << (results.max_drawdown * 100) << "%" << std::endl;
              
    std::cout << std::left << std::setw(30) << "Win Rate:"
              << std::right << std::fixed << std::setprecision(2)
              << (results.win_rate * 100) << "%" << std::endl;
              
    std::cout << std::left << std::setw(30) << "Total Trades:"
              << std::right << results.total_trades << std::endl;
              
    std::cout << std::left << std::setw(30) << "Winning Trades:"
              << std::right << results.winning_trades << std::endl;
              
    std::cout << std::left << std::setw(30) << "Losing Trades:"
              << std::right << results.losing_trades << std::endl;
              
    std::cout << std::left << std::setw(30) << "Profit Factor:"
              << std::right << std::fixed << std::setprecision(3)
              << results.profit_factor << std::endl;
              
    std::cout << std::string(80, '-') << std::endl;
}

int main() {
    std::cout << R"(
╔════════════════════════════════════════════════════════════════════════════╗
║              Trading Bot with API Integration - Full Demo                  ║
║              Fetch Real Market Data & Run Backtests                        ║
╚════════════════════════════════════════════════════════════════════════════╝
)" << std::endl;

    try {
        // Initialize TradingBot
        print_separator();
        std::cout << "STEP 1: Initializing Trading Bot" << std::endl;
        print_separator();
        
        TradingBot::TradingBot bot;
        if (!bot.initialize("config.json")) {
            std::cout << "❌ Failed to initialize TradingBot" << std::endl;
            return 1;
        }
        std::cout << "✓ TradingBot initialized successfully" << std::endl;
        
        // Test 1: Fetch data for multiple symbols
        print_separator();
        std::cout << "STEP 2: Fetching Real Market Data" << std::endl;
        print_separator();
        
        std::vector<std::string> symbols = {"AAPL", "MSFT", "GOOGL"};
        std::string start_date = "2024-01-01";
        std::string end_date = "2024-10-07";
        
        std::cout << "\nFetching data from " << start_date << " to " << end_date << "\n" << std::endl;
        
        for (const auto& symbol : symbols) {
            std::cout << "Fetching " << symbol << "... ";
            std::string output_file = "data/" + symbol + "_historical.csv";
            
            if (bot.fetch_market_data(symbol, start_date, end_date, output_file)) {
                std::cout << "✓ Saved to " << output_file << std::endl;
            } else {
                std::cout << "❌ Failed" << std::endl;
            }
        }
        
        // Test 2: Run backtest with API-fetched data (no CSV file needed)
        print_separator();
        std::cout << "\nSTEP 3: Running Backtests with Live API Data" << std::endl;
        print_separator();
        
        std::vector<std::string> strategies = {"SMA_CROSSOVER", "EMA_CROSSOVER", "RSI"};
        std::string test_symbol = "AAPL";
        
        std::cout << "\nTesting " << test_symbol << " with multiple strategies\n" << std::endl;
        
        for (const auto& strategy : strategies) {
            std::cout << "\n--- Testing " << strategy << " Strategy ---" << std::endl;
            
            // Run backtest directly from API (no manual CSV download needed)
            if (bot.run_backtest_with_api(test_symbol, strategy, start_date, end_date)) {
                std::cout << "✓ Backtest completed successfully" << std::endl;
                
                // Print results
                const auto& results = bot.get_results();
                print_results(results);
                
                // Generate HTML report
                std::string report_file = test_symbol + "_" + strategy + "_report.html";
                bot.generate_report(report_file);
                std::cout << "✓ Report saved: " << report_file << std::endl;
                
            } else {
                std::cout << "❌ Backtest failed" << std::endl;
            }
            
            std::cout << "\nPress Enter to continue to next strategy...";
            std::cin.get();
        }
        
        // Test 3: Comparison across multiple symbols
        print_separator();
        std::cout << "\nSTEP 4: Multi-Symbol Strategy Comparison" << std::endl;
        print_separator();
        
        std::cout << "\nComparing SMA_CROSSOVER strategy across different symbols\n" << std::endl;
        
        struct StrategyResult {
            std::string symbol;
            double total_return;
            int total_trades;
            double win_rate;
        };
        
        std::vector<StrategyResult> comparison_results;
        
        for (const auto& symbol : symbols) {
            std::cout << "\nTesting " << symbol << "..." << std::endl;
            
            if (bot.run_backtest_with_api(symbol, "SMA_CROSSOVER", start_date, end_date)) {
                const auto& results = bot.get_results();
                
                StrategyResult sr;
                sr.symbol = symbol;
                sr.total_return = results.total_return;
                sr.total_trades = results.total_trades;
                sr.win_rate = results.win_rate;
                comparison_results.push_back(sr);
                
                std::cout << "  Return: " << std::fixed << std::setprecision(2)
                          << (results.total_return * 100) << "% | "
                          << "Trades: " << results.total_trades << " | "
                          << "Win Rate: " << (results.win_rate * 100) << "%" << std::endl;
            } else {
                std::cout << "  ❌ Failed" << std::endl;
            }
        }
        
        // Print comparison summary
        if (!comparison_results.empty()) {
            std::cout << "\n" << std::string(80, '-') << std::endl;
            std::cout << "STRATEGY COMPARISON SUMMARY" << std::endl;
            std::cout << std::string(80, '-') << std::endl;
            
            std::cout << std::left << std::setw(15) << "Symbol"
                      << std::right << std::setw(20) << "Total Return"
                      << std::setw(20) << "Total Trades"
                      << std::setw(20) << "Win Rate" << std::endl;
            std::cout << std::string(80, '-') << std::endl;
            
            for (const auto& result : comparison_results) {
                std::cout << std::left << std::setw(15) << result.symbol
                          << std::right << std::setw(19) << std::fixed << std::setprecision(2)
                          << (result.total_return * 100) << "%"
                          << std::setw(20) << result.total_trades
                          << std::setw(19) << std::fixed << std::setprecision(2)
                          << (result.win_rate * 100) << "%" << std::endl;
            }
            std::cout << std::string(80, '-') << std::endl;
        }
        
        print_separator();
        std::cout << "\n✓ ALL TESTS COMPLETED SUCCESSFULLY!" << std::endl;
        print_separator();
        
        std::cout << "\n📊 What You Can Do Now:\n" << std::endl;
        std::cout << "1. Check the generated HTML reports for detailed analysis" << std::endl;
        std::cout << "2. Review the downloaded CSV files in the data/ directory" << std::endl;
        std::cout << "3. Modify config.json to test different parameters" << std::endl;
        std::cout << "4. Add your own custom strategies" << std::endl;
        std::cout << "5. Test with different date ranges and intervals" << std::endl;
        std::cout << "6. Get a free Alpha Vantage API key for more features:" << std::endl;
        std::cout << "   https://www.alphavantage.co/support/#api-key" << std::endl;
        
        print_separator();
        std::cout << "\n🎉 Your trading bot is now fully operational with live data!" << std::endl;
        print_separator();
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

