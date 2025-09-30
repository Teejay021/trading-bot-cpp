#include "trading_bot.h"
#include "utils/logger.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

// For JSON parsing (simple approach)
#include <map>
#include <string>

namespace TradingBot {

TradingBot::TradingBot() {
    // Initialize all component pointers to nullptr
    // They will be created during initialization
}

TradingBot::~TradingBot() {
    // Unique pointers automatically clean up
}

bool TradingBot::initialize(const std::string& config_file) {
    try {        
        // Load configuration first
        if (!load_configuration(config_file)) {
            LOG_WARNING("Using default configuration due to config load failure");
        }
        
        // Initialize CSV Parser
        csv_parser_ = std::make_unique<CSVParser>();
        
        // Initialize Risk Manager with loaded or default parameters
        risk_manager_ = std::make_unique<RiskManager>();
        RiskParameters risk_params = load_risk_parameters();
        if (!risk_manager_->initialize(risk_params)) {
            LOG_ERROR("Failed to initialize Risk Manager");
            return false;
        }
        
        // Initialize Backtester with loaded or default configuration
        backtester_ = std::make_unique<Backtester>();
        BacktestConfig backtest_config = load_backtest_config();
        if (!backtester_->initialize(backtest_config)) {
            LOG_ERROR("Failed to initialize Backtester");
            return false;
        }
        
        // Initialize Report Generator (placeholder for now)
        // report_generator_ = std::make_unique<ReportGenerator>();
        
        LOG_INFO("TradingBot initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to initialize TradingBot: " + std::string(e.what()));
        return false;
    }
}

bool TradingBot::run_backtest(const std::string& data_file, const std::string& strategy_name) {
    try {
        
        if (!csv_parser_->load_data(data_file)) {
            LOG_ERROR("Failed to load data from: " + data_file);
            return false;
        }
        
        if (!csv_parser_->validate_data()) {
            LOG_ERROR("Data validation failed for: " + data_file);
            return false;
        }
        
        LOG_INFO("Loaded " + std::to_string(csv_parser_->get_data_count()) + " rows of market data");
        
        
        strategy_ = create_strategy(strategy_name);
        if (!strategy_) {
            LOG_ERROR("Failed to create strategy: " + strategy_name);
            return false;
        }
        
        // Initialize strategy with default parameters
        auto strategy_params = get_strategy_parameters(strategy_name);
        if (!strategy_->initialize(strategy_params)) {
            LOG_ERROR("Failed to initialize strategy: " + strategy_name);
            return false;
        }
        
        LOG_INFO("Initialized strategy: " + strategy_name);
        
        
        results_ = backtester_->run_backtest(strategy_, csv_parser_, risk_manager_);
        
        LOG_INFO("Backtest completed successfully");
        LOG_INFO("Total trades: " + std::to_string(results_.total_trades));
        LOG_INFO("Total return: " + std::to_string(results_.total_return * 100) + "%");
        
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("Backtest failed: " + std::string(e.what()));
        return false;
    }
}

void TradingBot::generate_report(const std::string& output_file) {
    try {
        // TODO: Use ReportGenerator when implemented
        
        std::ofstream report(output_file);
        if (!report.is_open()) {
            LOG_ERROR("Failed to create report file: " + output_file);
            return;
        }
        
        
        report << "<!DOCTYPE html>\n";
        report << "<html><head><title>Trading Bot Backtest Report</title></head>\n";
        report << "<body>\n";
        report << "<h1>Backtest Results</h1>\n";
        report << "<table border='1'>\n";
        report << "<tr><th>Metric</th><th>Value</th></tr>\n";
        report << "<tr><td>Total Return</td><td>" << (results_.total_return * 100) << "%</td></tr>\n";
        report << "<tr><td>Annualized Return</td><td>" << (results_.annualized_return * 100) << "%</td></tr>\n";
        report << "<tr><td>Sharpe Ratio</td><td>" << results_.sharpe_ratio << "</td></tr>\n";
        report << "<tr><td>Max Drawdown</td><td>" << (results_.max_drawdown * 100) << "%</td></tr>\n";
        report << "<tr><td>Win Rate</td><td>" << (results_.win_rate * 100) << "%</td></tr>\n";
        report << "<tr><td>Total Trades</td><td>" << results_.total_trades << "</td></tr>\n";
        report << "<tr><td>Winning Trades</td><td>" << results_.winning_trades << "</td></tr>\n";
        report << "<tr><td>Losing Trades</td><td>" << results_.losing_trades << "</td></tr>\n";
        report << "<tr><td>Profit Factor</td><td>" << results_.profit_factor << "</td></tr>\n";
        report << "</table>\n";
        report << "</body></html>\n";
        
        report.close();
        LOG_INFO("Report generated: " + output_file);
        
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to generate report: " + std::string(e.what()));
    }
}

const BacktestResults& TradingBot::get_results() const {
    return results_;
}

// Private helper methods

std::unique_ptr<Strategy> TradingBot::create_strategy(const std::string& strategy_name) {
    // Factory method to create different strategy types
    
    if (strategy_name == "SMA_CROSSOVER" || strategy_name == "SMA") {
        return std::make_unique<SMACrossoverStrategy>();
        
    } else if (strategy_name == "EMA_CROSSOVER" || strategy_name == "EMA") {
        return std::make_unique<EMAStrategy>();
        
    } else if (strategy_name == "RSI" || strategy_name == "RSI_STRATEGY") {
        return std::make_unique<RSIStrategy>();
        
    } else {
        LOG_ERROR("Unknown strategy name: " + strategy_name);
        LOG_INFO("Available strategies: SMA_CROSSOVER, EMA_CROSSOVER, RSI");
        return nullptr;
    }
}

std::map<std::string, double> TradingBot::get_strategy_parameters(const std::string& strategy_name) {
    std::map<std::string, double> params;
    
    // Start with default parameters for each strategy type
    if (strategy_name == "SMA_CROSSOVER" || strategy_name == "SMA") {
        params = {
            {"short_period", 10.0},
            {"long_period", 30.0}
        };
        
        // Override with config file values if available
        try {
            if (config_data_.find("strategies") != config_data_.end()) {
                auto& strategies = config_data_["strategies"];
                if (strategies.find("SMA_CROSSOVER") != strategies.end()) {
                    auto& sma_config = strategies["SMA_CROSSOVER"];
                    if (sma_config.find("short_period") != sma_config.end()) {
                        params["short_period"] = std::stod(sma_config["short_period"]);
                    }
                    if (sma_config.find("long_period") != sma_config.end()) {
                        params["long_period"] = std::stod(sma_config["long_period"]);
                    }
                }
            }
        } catch (const std::exception& e) {
            LOG_WARNING("Error loading SMA config, using defaults: " + std::string(e.what()));
        }
        
    } else if (strategy_name == "EMA_CROSSOVER" || strategy_name == "EMA") {
        params = {
            {"short_period", 12.0},
            {"long_period", 26.0}
        };
        
        // Override with config file values if available
        try {
            if (config_data_.find("strategies") != config_data_.end()) {
                auto& strategies = config_data_["strategies"];
                if (strategies.find("EMA_CROSSOVER") != strategies.end()) {
                    auto& ema_config = strategies["EMA_CROSSOVER"];
                    if (ema_config.find("short_period") != ema_config.end()) {
                        params["short_period"] = std::stod(ema_config["short_period"]);
                    }
                    if (ema_config.find("long_period") != ema_config.end()) {
                        params["long_period"] = std::stod(ema_config["long_period"]);
                    }
                }
            }
        } catch (const std::exception& e) {
            LOG_WARNING("Error loading EMA config, using defaults: " + std::string(e.what()));
        }
        
    } else if (strategy_name == "RSI" || strategy_name == "RSI_STRATEGY") {
        params = {
            {"rsi_period", 14.0},
            {"overbought_threshold", 70.0},
            {"oversold_threshold", 30.0}
        };
        
        // Override with config file values if available
        try {
            if (config_data_.find("strategies") != config_data_.end()) {
                auto& strategies = config_data_["strategies"];
                if (strategies.find("RSI") != strategies.end()) {
                    auto& rsi_config = strategies["RSI"];
                    if (rsi_config.find("rsi_period") != rsi_config.end()) {
                        params["rsi_period"] = std::stod(rsi_config["rsi_period"]);
                    }
                    if (rsi_config.find("overbought_threshold") != rsi_config.end()) {
                        params["overbought_threshold"] = std::stod(rsi_config["overbought_threshold"]);
                    }
                    if (rsi_config.find("oversold_threshold") != rsi_config.end()) {
                        params["oversold_threshold"] = std::stod(rsi_config["oversold_threshold"]);
                    }
                }
            }
        } catch (const std::exception& e) {
            LOG_WARNING("Error loading RSI config, using defaults: " + std::string(e.what()));
        }
        
    } else {
        LOG_ERROR("Unknown strategy name: " + strategy_name);
        LOG_INFO("Available strategies: SMA_CROSSOVER, EMA_CROSSOVER, RSI");
        return {};
    }
    
    return params;
}

bool TradingBot::load_configuration(const std::string& config_file) {
    try {
        std::ifstream file(config_file);
        if (!file.is_open()) {
            LOG_WARNING("Config file not found, using default configuration: " + config_file);
            return true; // Use defaults
        }
        
        // Read entire config file
        std::string config_content((std::istreambuf_iterator<char>(file)),
                                   std::istreambuf_iterator<char>());
        file.close();
        
        // Simple JSON-like parsing (basic implementation)
        config_data_ = parse_simple_json(config_content);
        
        LOG_INFO("Configuration loaded from: " + config_file);
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to load configuration: " + std::string(e.what()));
        return false;
    }
}

RiskParameters TradingBot::load_risk_parameters() {
    RiskParameters params; // Start with defaults
    
    try {
        // Override with config values if available
        if (config_data_.find("risk_management") != config_data_.end()) {
            auto& risk_config = config_data_["risk_management"];
            
            if (risk_config.find("max_position_size") != risk_config.end()) {
                params.max_position_size = std::stod(risk_config["max_position_size"]);
            }
            if (risk_config.find("max_drawdown") != risk_config.end()) {
                params.max_drawdown = std::stod(risk_config["max_drawdown"]);
            }
            if (risk_config.find("stop_loss_pct") != risk_config.end()) {
                params.stop_loss_pct = std::stod(risk_config["stop_loss_pct"]);
            }
            if (risk_config.find("take_profit_pct") != risk_config.end()) {
                params.take_profit_pct = std::stod(risk_config["take_profit_pct"]);
            }
            if (risk_config.find("max_daily_loss") != risk_config.end()) {
                params.max_daily_loss = std::stod(risk_config["max_daily_loss"]);
            }
            if (risk_config.find("position_sizing_atr") != risk_config.end()) {
                params.position_sizing_atr = std::stod(risk_config["position_sizing_atr"]);
            }
        }
    } catch (const std::exception& e) {
        LOG_WARNING("Error parsing risk parameters, using defaults: " + std::string(e.what()));
    }
    
    return params;
}

BacktestConfig TradingBot::load_backtest_config() {
    BacktestConfig config; // Start with defaults
    
    try {
        // Override with config values if available
        if (config_data_.find("backtesting") != config_data_.end()) {
            auto& backtest_config = config_data_["backtesting"];
            
            if (backtest_config.find("initial_capital") != backtest_config.end()) {
                config.initial_capital = std::stod(backtest_config["initial_capital"]);
            }
            if (backtest_config.find("commission_rate") != backtest_config.end()) {
                config.commission_rate = std::stod(backtest_config["commission_rate"]);
            }
            if (backtest_config.find("slippage") != backtest_config.end()) {
                config.slippage = std::stod(backtest_config["slippage"]);
            }
            if (backtest_config.find("enable_short_selling") != backtest_config.end()) {
                config.enable_short_selling = (backtest_config["enable_short_selling"] == "true");
            }
            if (backtest_config.find("start_date") != backtest_config.end()) {
                config.start_date = backtest_config["start_date"];
            }
            if (backtest_config.find("end_date") != backtest_config.end()) {
                config.end_date = backtest_config["end_date"];
            }
        }
    } catch (const std::exception& e) {
        LOG_WARNING("Error parsing backtest config, using defaults: " + std::string(e.what()));
    }
    
    return config;
}

std::map<std::string, std::map<std::string, std::string>> TradingBot::parse_simple_json(const std::string& json_content) {
    std::map<std::string, std::map<std::string, std::string>> result;
    
    // Simple JSON parser for basic key-value pairs
    // This is a basic implementation - for production use a proper JSON library
    
    std::string current_section;
    std::istringstream iss(json_content);
    std::string line;
    
    while (std::getline(iss, line)) {
        // Remove whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // Skip comments and empty lines
        if (line.empty() || line[0] == '/' || line[0] == '{' || line[0] == '}') {
            continue;
        }
        
        // Check for section header (e.g., "risk_management": {)
        if (line.find("\":") != std::string::npos && line.find("{") != std::string::npos) {
            size_t start = line.find("\"") + 1;
            size_t end = line.find("\"", start);
            current_section = line.substr(start, end - start);
            continue;
        }
        
        // Parse key-value pairs
        if (line.find(":") != std::string::npos && !current_section.empty()) {
            size_t colon = line.find(":");
            std::string key = line.substr(0, colon);
            std::string value = line.substr(colon + 1);
            
            // Clean up key and value
            key.erase(0, key.find_first_not_of(" \t\""));
            key.erase(key.find_last_not_of(" \t\",") + 1);
            value.erase(0, value.find_first_not_of(" \t\""));
            value.erase(value.find_last_not_of(" \t\",") + 1);
            
            result[current_section][key] = value;
        }
    }
    
    return result;
}

} // namespace TradingBot
