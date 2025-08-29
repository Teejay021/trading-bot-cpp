#include "data/csv_parser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

namespace TradingBot {

CSVParser::CSVParser() {
}

CSVParser::~CSVParser() {
}



bool CSVParser::load_data(const std::string& filename){

    std::ifstream file(filename);
    if(!file.is_open()){
        return false;
    }

    data_.clear();
    std::string line;

    std::getline(file,line); //skip header

    while(std::getline(file,line)){

        if(!line.empty()){

            MarketData data = parse_line(line);
            data_.push_back(data);
        }
    }


    return !data_.empty();
}

    // Get data at specific index
    const MarketData& CSVParser::get_data(size_t index) const {

        if(index >= data_.size()){
            throw std::out_of_range("Index out of range");
        }
        return data_[index];

    }
        
    // Get total number of data points
    size_t CSVParser::get_data_count() const{

        return data_.size();
    }
        
    // Get data range
    std::vector<MarketData> CSVParser::get_data_range(size_t start, size_t end) const {

        if(start >= data_.size() || end >= data_.size() || start > end){
            throw std::out_of_range("Invalid range");
        }

        return std::vector<MarketData>(data_.begin() + start, data_.begin() + end + 1);
    }
        
    // Validate data integrity
    bool CSVParser::validate_data() const{

        for(const auto& data : data_){

            if(data.open <= 0 || data.high <= 0 || data.low <= 0 || data.close <= 0 || data.volume <= 0){

                return false;
            }

            if (data.high < data.open || data.high < data.close || data.high < data.low) {
                return false;
            }

            if (data.low > data.open || data.low > data.close || data.low > data.high) {
                return false;
            }
        }

        return true;
    }
        
    
    void CSVParser::clear(){

        data_.clear();

    }


    // Helper function to parse a single line of CSV
    MarketData CSVParser::parse_line(const std::string&line){

        std::stringstream ss(line);
        std::string token;
        MarketData data;
        size_t field_count = 0;

        while(std::getline(ss,token,',')){

            switch(field_count){
                case 0:
                    data.timestamp = token;
                    break;
                case 1:
                    data.open = parse_double(token);
                    break;
                case 2:
                    data.high = parse_double(token);
                    break;
                case 3:
                    data.low = parse_double(token);
                    break;
                case 4:
                    data.close = parse_double(token);
                    break;
                case 5:
                    data.volume = parse_double(token);
        }

        field_count++;

    }

    return data;

}

double CSVParser::parse_double(const std::string&str){
    try{
        return std::stod(str);
    }catch(const std::invalid_argument&e){
        return 0.0;
    }catch(const std::out_of_range&e){
        return 0.0;
    }
}

}
