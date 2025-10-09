#include <iostream>
#include <iomanip>
#include <chrono>
#include "data/api_data_fetcher.h"

using namespace TradingBot;

void print_separator() {
    std::cout << std::string(80, '=') << std::endl;
}

void print_market_data(const std::vector<MarketData>& data, int max_rows = 10) {
    std::cout << std::left << std::setw(20) << "Timestamp"
              << std::right << std::setw(12) << "Open"
              << std::setw(12) << "High"
              << std::setw(12) << "Low"
              << std::setw(12) << "Close"
              << std::setw(12) << "Volume" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    
    int count = 0;
    for (const auto& d : data) {
        if (count >= max_rows && count < static_cast<int>(data.size()) - 5) {
            if (count == max_rows) {
                std::cout << "... (" << (data.size() - max_rows - 5) << " rows omitted) ..." << std::endl;
            }
            count++;
            continue;
        }
        
        std::cout << std::left << std::setw(20) << d.timestamp
                  << std::right << std::fixed << std::setprecision(2)
                  << std::setw(12) << d.open
                  << std::setw(12) << d.high
                  << std::setw(12) << d.low
                  << std::setw(12) << d.close
                  << std::setw(12) << std::fixed << std::setprecision(0) << d.volume << std::endl;
        count++;
    }
}

void test_yahoo_finance() {
    print_separator();
    std::cout << "TEST 1: Yahoo Finance API" << std::endl;
    print_separator();
    
    APIDataFetcher fetcher;
    
    // Initialize with Yahoo Finance (no API key needed)
    std::map<std::string, std::string> config;
    config["default_provider"] = "yahoo_finance";
    
    if (!fetcher.initialize(config)) {
        std::cout << "❌ Failed to initialize API fetcher" << std::endl;
        return;
    }
    
    std::cout << "✓ Initialized successfully" << std::endl;
    
    // Set Yahoo Finance as provider
    fetcher.set_provider(APIProvider::YAHOO_FINANCE);
    std::cout << "✓ Using Yahoo Finance provider" << std::endl;
    
    // Fetch Apple stock data for the last 3 months
    std::string symbol = "AAPL";
    std::string end_date = "2024-10-07";
    std::string start_date = "2024-07-07";
    
    std::cout << "\nFetching " << symbol << " data from " << start_date << " to " << end_date << "..." << std::endl;
    
    APIResponse response = fetcher.fetch_data(
        symbol,
        DataInterval::DAILY,
        start_date,
        end_date
    );
    
    if (response.success) {
        std::cout << "✓ Successfully fetched " << response.data.size() << " data points" << std::endl;
        std::cout << "\nFirst and last 10 rows:" << std::endl;
        print_market_data(response.data, 10);
        
        // Save to CSV
        std::string filename = "yahoo_" + symbol + "_daily.csv";
        if (fetcher.save_to_csv(response, filename)) {
            std::cout << "\n✓ Data saved to: " << filename << std::endl;
        }
    } else {
        std::cout << "❌ Failed to fetch data: " << response.error_message << std::endl;
    }
}

void test_alpha_vantage() {
    print_separator();
    std::cout << "TEST 2: Alpha Vantage API" << std::endl;
    print_separator();
    
    APIDataFetcher fetcher;
    
    // Initialize with Alpha Vantage
    // Note: Using "demo" key which has limited functionality
    // Get your free key from: https://www.alphavantage.co/support/#api-key
    std::map<std::string, std::string> config;
    config["alpha_vantage_key"] = "demo";
    
    if (!fetcher.initialize(config)) {
        std::cout << "❌ Failed to initialize API fetcher" << std::endl;
        return;
    }
    
    std::cout << "✓ Initialized successfully" << std::endl;
    
    // Set Alpha Vantage as provider
    if (!fetcher.set_provider(APIProvider::ALPHA_VANTAGE)) {
        std::cout << "❌ Failed to set Alpha Vantage provider" << std::endl;
        return;
    }
    
    std::cout << "✓ Using Alpha Vantage provider" << std::endl;
    std::cout << "⚠ Note: Using demo API key (limited to IBM symbol)" << std::endl;
    
    // Fetch IBM stock data (demo key only works with IBM)
    std::string symbol = "IBM";
    std::string end_date = "2024-10-07";
    std::string start_date = "2024-09-01";
    
    std::cout << "\nFetching " << symbol << " data from " << start_date << " to " << end_date << "..." << std::endl;
    
    APIResponse response = fetcher.fetch_data(
        symbol,
        DataInterval::DAILY,
        start_date,
        end_date
    );
    
    if (response.success) {
        std::cout << "✓ Successfully fetched " << response.data.size() << " data points" << std::endl;
        std::cout << "\nFirst and last 10 rows:" << std::endl;
        print_market_data(response.data, 10);
        
        // Save to CSV
        std::string filename = "alphavantage_" + symbol + "_daily.csv";
        if (fetcher.save_to_csv(response, filename)) {
            std::cout << "\n✓ Data saved to: " << filename << std::endl;
        }
    } else {
        std::cout << "❌ Failed to fetch data: " << response.error_message << std::endl;
        std::cout << "\nNote: If you see rate limit errors, get a free API key from:" << std::endl;
        std::cout << "https://www.alphavantage.co/support/#api-key" << std::endl;
    }
}

void test_multiple_symbols() {
    print_separator();
    std::cout << "TEST 3: Multiple Symbols" << std::endl;
    print_separator();
    
    APIDataFetcher fetcher;
    
    std::map<std::string, std::string> config;
    if (!fetcher.initialize(config)) {
        std::cout << "❌ Failed to initialize API fetcher" << std::endl;
        return;
    }
    
    fetcher.set_provider(APIProvider::YAHOO_FINANCE);
    
    std::vector<std::string> symbols = {"AAPL", "MSFT", "GOOGL", "TSLA", "AMZN"};
    std::string end_date = "2024-10-07";
    std::string start_date = "2024-09-07";
    
    std::cout << "Fetching recent data for multiple symbols..." << std::endl;
    
    for (const auto& symbol : symbols) {
        std::cout << "\n" << symbol << ": ";
        
        APIResponse response = fetcher.fetch_data(
            symbol,
            DataInterval::DAILY,
            start_date,
            end_date
        );
        
        if (response.success && !response.data.empty()) {
            const auto& latest = response.data.back();
            std::cout << "✓ " << response.data.size() << " days | "
                      << "Latest close: $" << std::fixed << std::setprecision(2) << latest.close;
            
            // Save to CSV
            std::string filename = "data_" + symbol + "_daily.csv";
            fetcher.save_to_csv(response, filename);
        } else {
            std::cout << "❌ " << response.error_message;
        }
    }
    
    std::cout << "\n\n✓ All data files saved to current directory" << std::endl;
}

void test_caching() {
    print_separator();
    std::cout << "TEST 4: Data Caching" << std::endl;
    print_separator();
    
    APIDataFetcher fetcher;
    
    std::map<std::string, std::string> config;
    if (!fetcher.initialize(config)) {
        std::cout << "❌ Failed to initialize API fetcher" << std::endl;
        return;
    }
    
    fetcher.set_provider(APIProvider::YAHOO_FINANCE);
    fetcher.enable_caching(true);
    
    std::string symbol = "AAPL";
    std::string end_date = "2024-10-07";
    std::string start_date = "2024-09-07";
    
    std::cout << "First fetch (from API)..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    APIResponse response1 = fetcher.fetch_data(symbol, DataInterval::DAILY, start_date, end_date);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    if (response1.success) {
        std::cout << "✓ Fetched " << response1.data.size() << " points in " << duration1 << "ms" << std::endl;
    }
    
    std::cout << "\nSecond fetch (from cache)..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    APIResponse response2 = fetcher.fetch_data(symbol, DataInterval::DAILY, start_date, end_date);
    end = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    if (response2.success) {
        std::cout << "✓ Fetched " << response2.data.size() << " points in " << duration2 << "ms" << std::endl;
        std::cout << "⚡ Cache speedup: " << (duration1 - duration2) << "ms faster!" << std::endl;
    }
}

void print_usage() {
    std::cout << "\nAPI Data Fetcher - Usage Examples" << std::endl;
    print_separator();
    std::cout << "\n1. Get a FREE Alpha Vantage API key:" << std::endl;
    std::cout << "   Visit: https://www.alphavantage.co/support/#api-key" << std::endl;
    std::cout << "   Update config.json with your key" << std::endl;
    
    std::cout << "\n2. Yahoo Finance (No API key required):" << std::endl;
    std::cout << "   - Works immediately" << std::endl;
    std::cout << "   - Supports most major stocks" << std::endl;
    std::cout << "   - Daily, weekly, and monthly data" << std::endl;
    
    std::cout << "\n3. Supported Symbols:" << std::endl;
    std::cout << "   - US Stocks: AAPL, MSFT, GOOGL, TSLA, AMZN, etc." << std::endl;
    std::cout << "   - ETFs: SPY, QQQ, IWM, etc." << std::endl;
    std::cout << "   - Crypto (with -USD): BTC-USD, ETH-USD, etc." << std::endl;
    
    print_separator();
}

int main() {
    std::cout << R"(
╔════════════════════════════════════════════════════════════════════════════╗
║                    Trading Bot - API Data Fetcher Test                     ║
║                    Fetch Real Market Data from APIs                        ║
╚════════════════════════════════════════════════════════════════════════════╝
)" << std::endl;

    print_usage();
    
    try {
        // Run tests
        test_yahoo_finance();
        std::cout << "\n\nPress Enter to continue...";
        std::cin.get();
        
        test_multiple_symbols();
        std::cout << "\n\nPress Enter to continue...";
        std::cin.get();
        
        test_caching();
        std::cout << "\n\nPress Enter to continue to Alpha Vantage test...";
        std::cin.get();
        
        test_alpha_vantage();
        
        print_separator();
        std::cout << "\n✓ ALL TESTS COMPLETED!" << std::endl;
        std::cout << "\nNext Steps:" << std::endl;
        std::cout << "1. Check the generated CSV files in the current directory" << std::endl;
        std::cout << "2. Use these CSV files with your trading bot backtests" << std::endl;
        std::cout << "3. Get your free Alpha Vantage API key for more features" << std::endl;
        std::cout << "4. Integrate API fetching directly into your TradingBot class" << std::endl;
        print_separator();
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

