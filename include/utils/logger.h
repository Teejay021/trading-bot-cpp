#pragma once

#include <string>
#include <fstream>
#include <memory>
#include <mutex>

namespace TradingBot {

    // Log levels
    enum class LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };

    // Logger configuration
    struct LoggerConfig {
        std::string log_file;
        LogLevel min_level;
        bool console_output;
        bool file_output;
        bool timestamp;
        int max_file_size_mb;
        
        LoggerConfig() : 
            log_file("./logs/trading_bot.log"), min_level(LogLevel::INFO),
            console_output(true), file_output(true), timestamp(true), max_file_size_mb(10)
        {}
    };

    // Logger class
    class Logger {
    public:
        Logger();
        ~Logger();
        
        // Initialize logger
        bool initialize(const LoggerConfig& config);
        
        // Log methods for different levels
        void debug(const std::string& message);
        void info(const std::string& message);
        void warning(const std::string& message);
        void error(const std::string& message);
        void critical(const std::string& message);
        
        // Generic log method
        void log(LogLevel level, const std::string& message);
        
        // Set log level
        void set_log_level(LogLevel level);
        
        // Get current log level
        LogLevel get_log_level() const;
        
        // Flush log buffer
        void flush();
        
        // Close logger
        void close();
        
        // Get logger configuration
        const LoggerConfig& get_config() const;
        
    private:
        LoggerConfig config_;
        std::ofstream log_file_;
        std::mutex log_mutex_;
        LogLevel current_level_;
        
        // Helper methods
        std::string level_to_string(LogLevel level);
        std::string get_timestamp();
        bool should_log(LogLevel level);
        void write_to_file(const std::string& message);
        void write_to_console(const std::string& message);
        void rotate_log_file();
        std::string format_message(LogLevel level, const std::string& message);
    };

    // Global logger instance
    extern std::unique_ptr<Logger> g_logger;
    
    // Convenience macros for logging
    #define LOG_DEBUG(msg) if(g_logger) g_logger->debug(msg)
    #define LOG_INFO(msg) if(g_logger) g_logger->info(msg)
    #define LOG_WARNING(msg) if(g_logger) g_logger->warning(msg)
    #define LOG_ERROR(msg) if(g_logger) g_logger->error(msg)
    #define LOG_CRITICAL(msg) if(g_logger) g_logger->critical(msg)

} // namespace TradingBot
