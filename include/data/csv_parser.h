#pragma once

#include <string>
#include <vector>
#include <memory>
#include <fstream>

namespace TradingBot {

    // Market data structure
    struct MarketData {
        std::string timestamp;
        double open;
        double high;
        double low;
        double close;
        double volume;
        
        MarketData() : open(0.0), high(0.0), low(0.0), close(0.0), volume(0.0) {}
    };

    // CSV Parser class for reading market data
    class CSVParser {
    public:
        CSVParser();
        ~CSVParser();
        
        // Load data from CSV file
        bool load_data(const std::string& filename);
        
        // Get data at specific index
        const MarketData& get_data(size_t index) const;
        
        // Get total number of data points
        size_t get_data_count() const;
        
        // Get data range
        std::vector<MarketData> get_data_range(size_t start, size_t end) const;
        
        // Validate data integrity
        bool validate_data() const;
        
        // Clear loaded data
        void clear();
        
    private:
        std::vector<MarketData> data_;
        
        // Parse single line of CSV
        MarketData parse_line(const std::string& line);
        
        // Convert string to double with error handling
        double parse_double(const std::string& str);
    };


} // namespace TradingBot
