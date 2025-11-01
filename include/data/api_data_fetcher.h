#pragma once

#include "data/csv_parser.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace TradingBot {

// MarketData is already defined in csv_parser.h

// API response structure
struct APIResponse {
    bool success;
    std::string error_message;
    std::vector<MarketData> data;
    std::map<std::string, std::string> metadata;
};

// API provider types
enum class APIProvider {
    ALPHA_VANTAGE,
    YAHOO_FINANCE,
    POLYGON_IO,
    IEX_CLOUD
};

// Data interval types
enum class DataInterval {
    MINUTE_1,
    MINUTE_5,
    MINUTE_15,
    MINUTE_30,
    HOUR_1,
    DAILY,
    WEEKLY,
    MONTHLY
};

// Base class for API clients
class APIClient {
public:
    virtual ~APIClient() = default;
    
    // Fetch historical data for a symbol
    virtual APIResponse fetch_historical_data(
        const std::string& symbol,
        DataInterval interval,
        const std::string& start_date,
        const std::string& end_date
    ) = 0;
    
    // Fetch latest quote
    virtual APIResponse fetch_latest_quote(const std::string& symbol) = 0;
    
    // Get provider name
    virtual std::string get_provider_name() const = 0;
    
    // Check if API key is valid
    virtual bool validate_api_key() = 0;
};

// Alpha Vantage API client
class AlphaVantageClient : public APIClient {
public:
    explicit AlphaVantageClient(const std::string& api_key);
    
    APIResponse fetch_historical_data(
        const std::string& symbol,
        DataInterval interval,
        const std::string& start_date,
        const std::string& end_date
    ) override;
    
    APIResponse fetch_latest_quote(const std::string& symbol) override;
    
    std::string get_provider_name() const override { return "Alpha Vantage"; }
    
    bool validate_api_key() override;
    
private:
    std::string api_key_;
    std::string base_url_;
    
    // Helper methods
    std::string make_request(const std::string& url);
    APIResponse parse_daily_response(const std::string& json_response);
    APIResponse parse_intraday_response(const std::string& json_response);
    std::string interval_to_string(DataInterval interval);
};

// Yahoo Finance API client (no API key needed)
class YahooFinanceClient : public APIClient {
public:
    YahooFinanceClient();
    
    APIResponse fetch_historical_data(
        const std::string& symbol,
        DataInterval interval,
        const std::string& start_date,
        const std::string& end_date
    ) override;
    
    APIResponse fetch_latest_quote(const std::string& symbol) override;
    
    std::string get_provider_name() const override { return "Yahoo Finance"; }
    
    bool validate_api_key() override { return true; } // No API key needed
    
private:
    std::string base_url_;
    
    std::string make_request(const std::string& url);
    APIResponse parse_csv_response(const std::string& csv_response);
    APIResponse parse_json_chart_response(const std::string& json_response);
    long long date_to_timestamp(const std::string& date);
};

// Main API Data Fetcher class
class APIDataFetcher {
public:
    APIDataFetcher();
    
    // Initialize with configuration
    bool initialize(const std::map<std::string, std::string>& config);
    
    // Set active provider
    bool set_provider(APIProvider provider);
    
    // Fetch data using active provider
    APIResponse fetch_data(
        const std::string& symbol,
        DataInterval interval,
        const std::string& start_date,
        const std::string& end_date
    );
    
    // Fetch latest quote
    APIResponse fetch_quote(const std::string& symbol);
    
    // Save fetched data to CSV
    bool save_to_csv(const APIResponse& response, const std::string& filename);
    
    // Get available providers
    std::vector<std::string> get_available_providers() const;
    
    // Enable/disable caching
    void enable_caching(bool enable) { caching_enabled_ = enable; }
    
    // Clear cache
    void clear_cache();
    
private:
    std::map<APIProvider, std::unique_ptr<APIClient>> clients_;
    APIProvider active_provider_;
    bool caching_enabled_;
    
    // Cache structure: symbol -> interval -> data
    std::map<std::string, std::map<DataInterval, APIResponse>> cache_;
    
    // Helper methods
    bool is_cached(const std::string& symbol, DataInterval interval) const;
    APIResponse get_cached_data(const std::string& symbol, DataInterval interval) const;
    void cache_data(const std::string& symbol, DataInterval interval, const APIResponse& response);
};

// Utility functions
namespace APIUtils {
    // HTTP GET request
    std::string http_get(const std::string& url);
    
    // URL encode
    std::string url_encode(const std::string& value);
    
    // Parse JSON string
    std::map<std::string, std::string> parse_json(const std::string& json);
    
    // Format date for API (YYYY-MM-DD)
    std::string format_date(const std::string& date);
    
    // Validate date format
    bool validate_date(const std::string& date);
}

} // namespace TradingBot

