#include "reporting/report_generator.h"
#include "utils/logger.h"
#include <fstream>
#include <sstream>

namespace TradingBot {
    
    bool ReportGenerator::initialize() {
        LOG_INFO("ReportGenerator initialized");
        return true;
    }
    
    bool ReportGenerator::generate_html_report(const BacktestResults& results, const std::string& output_file) {
        try {
            std::ofstream file(output_file);
            if (!file.is_open()) {
                LOG_ERROR("Failed to open output file: " + output_file);
                return false;
            }
            
            file << "<!DOCTYPE html>\n";
            file << "<html>\n<head>\n<title>Trading Bot Backtest Report</title>\n</head>\n<body>\n";
            file << "<h1>Trading Bot Backtest Report</h1>\n";
            file << "<h2>Performance Summary</h2>\n";
            file << "<p>Total Trades: " << results.total_trades << "</p>\n";
            file << "<p>Winning Trades: " << results.winning_trades << "</p>\n";
            file << "<p>Losing Trades: " << results.losing_trades << "</p>\n";
            file << "<p>Total Return: " << (results.total_return * 100) << "%</p>\n";
            file << "<p>Max Drawdown: " << (results.max_drawdown * 100) << "%</p>\n";
            file << "<p>Sharpe Ratio: " << results.sharpe_ratio << "</p>\n";
            file << "<p>Win Rate: " << (results.win_rate * 100) << "%</p>\n";
            file << "<p>Average Win: $" << results.avg_win << "</p>\n";
            file << "<p>Average Loss: $" << results.avg_loss << "</p>\n";
            file << "<p>Profit Factor: " << results.profit_factor << "</p>\n";
            file << "</body>\n</html>\n";
            
            file.close();
            LOG_INFO("HTML report generated: " + output_file);
            return true;
            
        } catch (const std::exception& e) {
            LOG_ERROR("Error generating HTML report: " + std::string(e.what()));
            return false;
        }
    }
    
    bool ReportGenerator::generate_csv_report(const BacktestResults& results, const std::string& output_file) {
        try {
            std::ofstream file(output_file);
            if (!file.is_open()) {
                LOG_ERROR("Failed to open output file: " + output_file);
                return false;
            }
            
            file << "Metric,Value\n";
            file << "Total Trades," << results.total_trades << "\n";
            file << "Winning Trades," << results.winning_trades << "\n";
            file << "Losing Trades," << results.losing_trades << "\n";
            file << "Total Return," << results.total_return << "\n";
            file << "Max Drawdown," << results.max_drawdown << "\n";
            file << "Sharpe Ratio," << results.sharpe_ratio << "\n";
            file << "Win Rate," << results.win_rate << "\n";
            file << "Average Win," << results.avg_win << "\n";
            file << "Average Loss," << results.avg_loss << "\n";
            file << "Profit Factor," << results.profit_factor << "\n";
            
            file.close();
            LOG_INFO("CSV report generated: " + output_file);
            return true;
            
        } catch (const std::exception& e) {
            LOG_ERROR("Error generating CSV report: " + std::string(e.what()));
            return false;
        }
    }
    
    std::string ReportGenerator::generate_summary(const BacktestResults& results) {
        std::stringstream ss;
        ss << "=== Backtest Summary ===\n";
        ss << "Total Trades: " << results.total_trades << "\n";
        ss << "Winning Trades: " << results.winning_trades << "\n";
        ss << "Losing Trades: " << results.losing_trades << "\n";
        ss << "Win Rate: " << (results.total_trades > 0 ? (results.winning_trades * 100.0 / results.total_trades) : 0) << "%\n";
        ss << "Total Return: " << (results.total_return * 100) << "%\n";
        ss << "Max Drawdown: " << (results.max_drawdown * 100) << "%\n";
        ss << "Sharpe Ratio: " << results.sharpe_ratio << "\n";
        ss << "Win Rate: " << (results.win_rate * 100) << "%\n";
        ss << "Average Win: $" << results.avg_win << "\n";
        ss << "Average Loss: $" << results.avg_loss << "\n";
        ss << "Profit Factor: " << results.profit_factor << "\n";
        return ss.str();
    }
    
}
