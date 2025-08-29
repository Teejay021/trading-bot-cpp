#include "trading_bot.h"
#include "utils/logger.h"
#include <iostream>
#include <memory>
#include <string>

int main(int argc, char* argv[]) {
    try {
        // Initialize global logger
        TradingBot::LoggerConfig logger_config;
        TradingBot::g_logger = std::make_unique<TradingBot::Logger>();
        if (!TradingBot::g_logger->initialize(logger_config)) {
            std::cerr << "Failed to initialize logger" << std::endl;
            return 1;
        }

        LOG_INFO("Starting Trading Bot...");

        // Create trading bot instance
        auto trading_bot = std::make_unique<TradingBot::TradingBot>();
        
        // Initialize the bot
        if (!trading_bot->initialize("config.json")) {
            LOG_ERROR("Failed to initialize trading bot");
            return 1;
        }

        LOG_INFO("Trading Bot initialized successfully");

        // Check command line arguments
        if (argc < 3) {
            std::cout << "Usage: " << argv[0] << " <data_file> <strategy_name>" << std::endl;
            std::cout << "Example: " << argv[0] << " data/SPY.csv SMA_CROSSOVER" << std::endl;
            return 1;
        }

        std::string data_file = argv[1];
        std::string strategy_name = argv[2];

        LOG_INFO("Running backtest with data file: " + data_file);
        LOG_INFO("Strategy: " + strategy_name);

        // Run backtest
        if (!trading_bot->run_backtest(data_file, strategy_name)) {
            LOG_ERROR("Backtest failed");
            return 1;
        }

        LOG_INFO("Backtest completed successfully");

        // Generate report
        std::string report_file = "backtest_report_" + strategy_name + ".html";
        trading_bot->generate_report(report_file);
        
        LOG_INFO("Report generated: " + report_file);

        // Display summary results
        const auto& results = trading_bot->get_results();
        std::cout << "\n=== Backtest Results ===" << std::endl;
        std::cout << "Total Return: " << (results.total_return * 100) << "%" << std::endl;
        std::cout << "Annualized Return: " << (results.annualized_return * 100) << "%" << std::endl;
        std::cout << "Sharpe Ratio: " << results.sharpe_ratio << std::endl;
        std::cout << "Max Drawdown: " << (results.max_drawdown * 100) << "%" << std::endl;
        std::cout << "Win Rate: " << (results.win_rate * 100) << "%" << std::endl;
        std::cout << "Total Trades: " << results.total_trades << std::endl;
        std::cout << "Profit Factor: " << results.profit_factor << std::endl;

        LOG_INFO("Trading Bot finished successfully");
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        if (TradingBot::g_logger) {
            TradingBot::g_logger->critical("Fatal error: " + std::string(e.what()));
        }
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        if (TradingBot::g_logger) {
            TradingBot::g_logger->critical("Unknown fatal error occurred");
        }
        return 1;
    }
}
