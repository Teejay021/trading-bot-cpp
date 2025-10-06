#include "utils/logger.h"
#include <iostream>

namespace TradingBot {
    // Global logger instance
    std::unique_ptr<Logger> g_logger = std::make_unique<Logger>();
    
    void Logger::info(const std::string& message) {
        std::cout << "[INFO] " << message << std::endl;
    }
    
    void Logger::warning(const std::string& message) {
        std::cout << "[WARNING] " << message << std::endl;
    }
    
    void Logger::error(const std::string& message) {
        std::cout << "[ERROR] " << message << std::endl;
    }
    
    void Logger::debug(const std::string& message) {
        std::cout << "[DEBUG] " << message << std::endl;
    }
}




