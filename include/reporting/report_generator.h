#pragma once
#include <string>
#include "backtester/backtester.h"

namespace TradingBot {
    class ReportGenerator {
    public:
        ReportGenerator() = default;
        ~ReportGenerator() = default;
        
        // Initialize the report generator
        bool initialize();
        
        // Generate HTML report
        bool generate_html_report(const BacktestResults& results, const std::string& output_file);
        
        // Generate CSV report
        bool generate_csv_report(const BacktestResults& results, const std::string& output_file);
        
        // Generate basic text summary
        std::string generate_summary(const BacktestResults& results);
    };
}