#include "data/csv_parser.h"
#include <iostream>

int main() {
    TradingBot::CSVParser parser;
    
    std::cout << "Testing CSV Parser..." << std::endl;
    
    // Load the sample data
    if (parser.load_data("data/sample_data.csv")) {
        std::cout << "✓ Successfully loaded CSV file" << std::endl;
        std::cout << "✓ Loaded " << parser.get_data_count() << " rows of data" << std::endl;
        
        // Validate the data
        if (parser.validate_data()) {
            std::cout << "✓ Data validation passed" << std::endl;
        } else {
            std::cout << "✗ Data validation failed" << std::endl;
        }
        
        // Show first few rows
        std::cout << "\nFirst 3 rows of data:" << std::endl;
        for (size_t i = 0; i < std::min(parser.get_data_count(), size_t(3)); ++i) {
            const auto& data = parser.get_data(i);
            std::cout << "Row " << i << ": " << data.timestamp 
                      << " | O:" << data.open 
                      << " H:" << data.high 
                      << " L:" << data.low 
                      << " C:" << data.close 
                      << " V:" << data.volume << std::endl;
        }
        
    } else {
        std::cout << "✗ Failed to load CSV file" << std::endl;
        return 1;
    }
    
    std::cout << "\nCSV Parser test completed!" << std::endl;
    return 0;
}
