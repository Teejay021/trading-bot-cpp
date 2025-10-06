#pragma once
#include <string>
#include <memory>

namespace TradingBot {
    class Logger {
    public:
        Logger() = default;
        ~Logger() = default;
        
        void info(const std::string& message);
        void warning(const std::string& message);
        void error(const std::string& message);
        void debug(const std::string& message);
    };
    
    // Global logger instance
    extern std::unique_ptr<Logger> g_logger;
    
    // Logging macros
    #define LOG_INFO(msg) if (g_logger) g_logger->info(msg)
    #define LOG_WARNING(msg) if (g_logger) g_logger->warning(msg)
    #define LOG_ERROR(msg) if (g_logger) g_logger->error(msg)
    #define LOG_DEBUG(msg) if (g_logger) g_logger->debug(msg)
}