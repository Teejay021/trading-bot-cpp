#include <iostream>
#include <memory>
#include <fstream>
#include <filesystem>
#include "trading_bot.h"
#include "utils/logger.h"

using namespace TradingBot;

// Test helper functions
bool create_test_config_file(const std::string& filename) {
    std::ofstream config(filename);
    if (!config.is_open()) {
        return false;
    }
    
    config << "{\n";
    config << "    \"trading_bot\": {\n";
    config << "        \"name\": \"Test_Trading_Bot\",\n";
    config << "        \"version\": \"1.0.0\"\n";
    config << "    },\n";
    config << "    \"backtesting\": {\n";
    config << "        \"initial_capital\": 50000.0,\n";
    config << "        \"commission_rate\": 0.001,\n";
    config << "        \"slippage\": 0.0001,\n";
    config << "        \"enable_short_selling\": false\n";
    config << "    },\n";
    config << "    \"risk_management\": {\n";
    config << "        \"max_position_size\": 0.03,\n";
    config << "        \"max_drawdown\": 0.15,\n";
    config << "        \"stop_loss_pct\": 0.04,\n";
    config << "        \"take_profit_pct\": 0.08,\n";
    config << "        \"max_daily_loss\": 0.05,\n";
    config << "        \"position_sizing_atr\": 2.5\n";
    config << "    },\n";
    config << "    \"strategies\": {\n";
    config << "        \"SMA_CROSSOVER\": {\n";
    config << "            \"short_period\": 5,\n";
    config << "            \"long_period\": 15\n";
    config << "        },\n";
    config << "        \"EMA_CROSSOVER\": {\n";
    config << "            \"short_period\": 8,\n";
    config << "            \"long_period\": 21\n";
    config << "        },\n";
    config << "        \"RSI\": {\n";
    config << "            \"rsi_period\": 10,\n";
    config << "            \"overbought_threshold\": 75,\n";
    config << "            \"oversold_threshold\": 25\n";
    config << "        }\n";
    config << "    }\n";
    config << "}\n";
    
    config.close();
    return true;
}

bool create_test_data_file(const std::string& filename) {
    std::ofstream data(filename);
    if (!data.is_open()) {
        return false;
    }
    
    // Create sample CSV data with realistic price movements
    data << "timestamp,open,high,low,close,volume\n";
    
    double base_price = 100.0;
    for (int i = 0; i < 100; ++i) {
        double open = base_price + (rand() % 200 - 100) / 100.0;
        double high = open + (rand() % 300) / 100.0;
        double low = open - (rand() % 300) / 100.0;
        double close = low + (rand() % static_cast<int>((high - low) * 100)) / 100.0;
        int volume = 100000 + rand() % 500000;
        
        data << "2023-01-" << (i % 28 + 1) << " 09:30:00,";
        data << open << "," << high << "," << low << "," << close << "," << volume << "\n";
        
        base_price = close; // Next day starts where previous ended
    }
    
    data.close();
    return true;
}

void cleanup_test_files() {
    std::remove("test_config.json");
    std::remove("test_data.csv");
    std::remove("test_report.html");
}

int main() {
    std::cout << "=== TradingBot Integration Test ===" << std::endl;
    
    // Initialize global logger for testing
    try {
        LoggerConfig logger_config;
        g_logger = std::make_unique<Logger>();
        if (!g_logger->initialize(logger_config)) {
            std::cout << "Warning: Logger initialization failed, continuing without logging" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Warning: Logger setup failed: " << e.what() << std::endl;
    }
    
    int tests_passed = 0;
    int total_tests = 0;
    
    // Test 1: TradingBot Construction
    total_tests++;
    std::cout << "\n--- Test 1: TradingBot Construction ---" << std::endl;
    try {
        TradingBot trading_bot;
        std::cout << "TradingBot created successfully" << std::endl;
        tests_passed++;
    } catch (const std::exception& e) {
        std::cout << "TradingBot construction failed: " << e.what() << std::endl;
    }
    
    // Test 2: Initialization with Default Config
    total_tests++;
    std::cout << "\n--- Test 2: Initialization with Default Config ---" << std::endl;
    try {
        TradingBot trading_bot;
        if (trading_bot.initialize("nonexistent_config.json")) {
            std::cout << "Initialization with default config successful" << std::endl;
            tests_passed++;
        } else {
            std::cout << "Initialization with default config failed" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Initialization test failed: " << e.what() << std::endl;
    }
    
    // Test 3: Initialization with Custom Config
    total_tests++;
    std::cout << "\n--- Test 3: Initialization with Custom Config ---" << std::endl;
    try {
        if (create_test_config_file("test_config.json")) {
            TradingBot trading_bot;
            if (trading_bot.initialize("test_config.json")) {
                std::cout << "Initialization with custom config successful" << std::endl;
                tests_passed++;
            } else {
                std::cout << "Initialization with custom config failed" << std::endl;
            }
        } else {
            std::cout << "Failed to create test config file" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Custom config test failed: " << e.what() << std::endl;
    }
    
    // Test 4: SMA Strategy Backtest
    total_tests++;
    std::cout << "\n--- Test 4: SMA Strategy Backtest ---" << std::endl;
    try {
        if (create_test_data_file("test_data.csv")) {
            TradingBot trading_bot;
            if (trading_bot.initialize("test_config.json")) {
                if (trading_bot.run_backtest("test_data.csv", "SMA_CROSSOVER")) {
                    const auto& results = trading_bot.get_results();
                    std::cout << "SMA backtest completed successfully" << std::endl;
                    std::cout << "   Total trades: " << results.total_trades << std::endl;
                    std::cout << "   Total return: " << (results.total_return * 100) << "%" << std::endl;
                    tests_passed++;
                } else {
                    std::cout << "SMA backtest execution failed" << std::endl;
                }
            } else {
                std::cout << "TradingBot initialization failed for SMA test" << std::endl;
            }
        } else {
            std::cout << "Failed to create test data file" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "SMA backtest test failed: " << e.what() << std::endl;
    }
    
    // Test 5: EMA Strategy Backtest
    total_tests++;
    std::cout << "\n--- Test 5: EMA Strategy Backtest ---" << std::endl;
    try {
        TradingBot trading_bot;
        if (trading_bot.initialize("test_config.json")) {
            if (trading_bot.run_backtest("test_data.csv", "EMA_CROSSOVER")) {
                const auto& results = trading_bot.get_results();
                std::cout << "EMA backtest completed successfully" << std::endl;
                std::cout << "   Total trades: " << results.total_trades << std::endl;
                std::cout << "   Win rate: " << (results.win_rate * 100) << "%" << std::endl;
                tests_passed++;
            } else {
                std::cout << "EMA backtest execution failed" << std::endl;
            }
        } else {
            std::cout << "TradingBot initialization failed for EMA test" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "EMA backtest test failed: " << e.what() << std::endl;
    }
    
    // Test 6: RSI Strategy Backtest
    total_tests++;
    std::cout << "\n--- Test 6: RSI Strategy Backtest ---" << std::endl;
    try {
        TradingBot trading_bot;
        if (trading_bot.initialize("test_config.json")) {
            if (trading_bot.run_backtest("test_data.csv", "RSI")) {
                const auto& results = trading_bot.get_results();
                std::cout << "RSI backtest completed successfully" << std::endl;
                std::cout << "   Total trades: " << results.total_trades << std::endl;
                std::cout << "   Max drawdown: " << (results.max_drawdown * 100) << "%" << std::endl;
                tests_passed++;
            } else {
                std::cout << "RSI backtest execution failed" << std::endl;
            }
        } else {
            std::cout << "TradingBot initialization failed for RSI test" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "RSI backtest test failed: " << e.what() << std::endl;
    }
    
    // Test 7: Report Generation
    total_tests++;
    std::cout << "\n--- Test 7: Report Generation ---" << std::endl;
    try {
        TradingBot trading_bot;
        if (trading_bot.initialize("test_config.json")) {
            if (trading_bot.run_backtest("test_data.csv", "SMA_CROSSOVER")) {
                trading_bot.generate_report("test_report.html");
                
                // Check if report file was created
                std::ifstream report_file("test_report.html");
                if (report_file.good()) {
                    std::cout << "Report generation successful" << std::endl;
                    tests_passed++;
                } else {
                    std::cout << "Report file was not created" << std::endl;
                }
                report_file.close();
            } else {
                std::cout << "Backtest failed, cannot test report generation" << std::endl;
            }
        } else {
            std::cout << "TradingBot initialization failed for report test" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Report generation test failed: " << e.what() << std::endl;
    }
    
    // Test 8: Invalid Strategy Name
    total_tests++;
    std::cout << "\n--- Test 8: Invalid Strategy Name Handling ---" << std::endl;
    try {
        TradingBot trading_bot;
        if (trading_bot.initialize("test_config.json")) {
            if (!trading_bot.run_backtest("test_data.csv", "INVALID_STRATEGY")) {
                std::cout << "Invalid strategy properly rejected" << std::endl;
                tests_passed++;
            } else {
                std::cout << "Invalid strategy was not rejected" << std::endl;
            }
        } else {
            std::cout << "TradingBot initialization failed for invalid strategy test" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Invalid strategy test failed: " << e.what() << std::endl;
    }
    
    // Test 9: Invalid Data File
    total_tests++;
    std::cout << "\n--- Test 9: Invalid Data File Handling ---" << std::endl;
    try {
        TradingBot trading_bot;
        if (trading_bot.initialize("test_config.json")) {
            if (!trading_bot.run_backtest("nonexistent_data.csv", "SMA_CROSSOVER")) {
                std::cout << "Invalid data file properly handled" << std::endl;
                tests_passed++;
            } else {
                std::cout << "Invalid data file was not properly handled" << std::endl;
            }
        } else {
            std::cout << "TradingBot initialization failed for invalid data test" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Invalid data file test failed: " << e.what() << std::endl;
    }
    
    // Cleanup test files
    cleanup_test_files();
    
    // Test Results Summary
    std::cout << "\n=== Test Results Summary ===" << std::endl;
    std::cout << "Tests Passed: " << tests_passed << "/" << total_tests << std::endl;
    std::cout << "Success Rate: " << (static_cast<double>(tests_passed) / total_tests * 100) << "%" << std::endl;
    
    if (tests_passed == total_tests) {
        std::cout << "All tests passed! TradingBot is working correctly." << std::endl;
        return 0;
    } else {
        std::cout << "Some tests failed. Please review the implementation." << std::endl;
        return 1;
    }
}
