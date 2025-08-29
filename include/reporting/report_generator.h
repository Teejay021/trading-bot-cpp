#pragma once

#include "backtester/backtester.h"
#include <string>
#include <vector>

namespace TradingBot {

    // Report types
    enum class ReportType {
        SUMMARY,
        DETAILED,
        TRADE_LOG,
        EQUITY_CURVE,
        PERFORMANCE_METRICS
    };

    // Report configuration
    struct ReportConfig {
        std::string output_format;     // CSV, JSON, HTML
        std::string output_directory;
        bool include_charts;
        bool include_trade_details;
        std::string chart_format;      // PNG, SVG, HTML
        
        ReportConfig() : 
            output_format("CSV"), output_directory("./reports"),
            include_charts(true), include_trade_details(true), chart_format("PNG")
        {}
    };

    // Report Generator class
    class ReportGenerator {
    public:
        ReportGenerator();
        ~ReportGenerator();
        
        // Initialize report generator
        bool initialize(const ReportConfig& config);
        
        // Generate summary report
        bool generate_summary_report(const BacktestResults& results, 
                                   const std::string& filename);
        
        // Generate detailed report
        bool generate_detailed_report(const BacktestResults& results,
                                    const std::string& filename);
        
        // Generate trade log
        bool generate_trade_log(const BacktestResults& results,
                               const std::string& filename);
        
        // Generate equity curve data
        bool generate_equity_curve(const BacktestResults& results,
                                  const std::string& filename);
        
        // Generate performance metrics
        bool generate_performance_metrics(const BacktestResults& results,
                                        const std::string& filename);
        
        // Generate all reports
        bool generate_all_reports(const BacktestResults& results,
                                 const std::string& base_filename);
        
        // Get report configuration
        const ReportConfig& get_config() const;
        
        // Set report configuration
        void set_config(const ReportConfig& config);
        
    private:
        ReportConfig config_;
        
        // Helper methods
        std::string format_percentage(double value);
        std::string format_currency(double value);
        std::string format_number(double value, int decimals = 2);
        bool create_output_directory();
        std::string get_timestamp();
        
        // CSV generation helpers
        bool write_csv_header(std::ofstream& file, const std::vector<std::string>& headers);
        bool write_csv_row(std::ofstream& file, const std::vector<std::string>& values);
        
        // HTML generation helpers
        std::string generate_html_header(const std::string& title);
        std::string generate_html_footer();
        std::string generate_performance_table(const BacktestResults& results);
        std::string generate_trade_table(const BacktestResults& results);
    };

} // namespace TradingBot
