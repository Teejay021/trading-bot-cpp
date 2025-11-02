#include "data/api_data_fetcher.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <stdexcept>
#include <cstring>

// For HTTP requests - using WinHTTP on Windows
#ifdef _WIN32
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#else
// For Unix-like systems, we'd use libcurl
#include <curl/curl.h>
#endif

namespace TradingBot {

// ============================================================================
// Utility Functions Implementation
// ============================================================================

namespace APIUtils {

#ifdef _WIN32
std::string http_get(const std::string& url) {
    std::string result;
    
    // Parse URL
    std::string host, path;
    bool is_https = (url.find("https://") == 0);
    size_t start = is_https ? 8 : 7;
    size_t slash_pos = url.find('/', start);
    
    if (slash_pos != std::string::npos) {
        host = url.substr(start, slash_pos - start);
        path = url.substr(slash_pos);
    } else {
        host = url.substr(start);
        path = "/";
    }
    
    // Convert host to wide string
    std::wstring wide_host(host.begin(), host.end());
    std::wstring wide_path(path.begin(), path.end());
    
    // Initialize WinHTTP
    HINTERNET hSession = WinHttpOpen(
        L"TradingBot/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0
    );
    
    if (!hSession) {
        return "";
    }
    
    // Connect to server
    HINTERNET hConnect = WinHttpConnect(
        hSession,
        wide_host.c_str(),
        is_https ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT,
        0
    );
    
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        return "";
    }
    
    // Open request
    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect,
        L"GET",
        wide_path.c_str(),
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        is_https ? WINHTTP_FLAG_SECURE : 0
    );
    
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }
    
    // Add headers to mimic a browser request
    // Note: Do NOT request gzip encoding as we don't have decompression support yet
    std::wstring headers = 
        L"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36\r\n"
        L"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
        L"Accept-Language: en-US,en;q=0.5\r\n"
        L"Connection: keep-alive\r\n";
    
    // Send request with headers
    BOOL bResults = WinHttpSendRequest(
        hRequest,
        headers.c_str(),
        -1,
        WINHTTP_NO_REQUEST_DATA,
        0,
        0,
        0
    );
    
    if (bResults) {
        bResults = WinHttpReceiveResponse(hRequest, NULL);
    }
    
    // Read data
    if (bResults) {
        DWORD dwSize = 0;
        DWORD dwDownloaded = 0;
        
        do {
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
                break;
            }
            
            if (dwSize == 0) {
                break;
            }
            
            char* pszOutBuffer = new char[dwSize + 1];
            ZeroMemory(pszOutBuffer, dwSize + 1);
            
            if (WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded)) {
                result.append(pszOutBuffer, dwDownloaded);
            }
            
            delete[] pszOutBuffer;
            
        } while (dwSize > 0);
    }
    
    // Cleanup
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    
    return result;
}
#else
// libcurl implementation for Unix-like systems
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string http_get(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // For testing
        
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        
        if (res != CURLE_OK) {
            return "";
        }
    }
    return readBuffer;
}
#endif

std::string url_encode(const std::string& value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;
    
    for (char c : value) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else {
            escaped << std::uppercase;
            escaped << '%' << std::setw(2) << int((unsigned char)c);
            escaped << std::nouppercase;
        }
    }
    
    return escaped.str();
}

std::string format_date(const std::string& date) {
    // Assumes input is already in YYYY-MM-DD format
    return date;
}

bool validate_date(const std::string& date) {
    if (date.length() != 10) return false;
    if (date[4] != '-' || date[7] != '-') return false;
    
    try {
        int year = std::stoi(date.substr(0, 4));
        int month = std::stoi(date.substr(5, 2));
        int day = std::stoi(date.substr(8, 2));
        
        if (year < 1900 || year > 2100) return false;
        if (month < 1 || month > 12) return false;
        if (day < 1 || day > 31) return false;
        
        return true;
    } catch (...) {
        return false;
    }
}

} // namespace APIUtils

// ============================================================================
// AlphaVantageClient Implementation
// ============================================================================

AlphaVantageClient::AlphaVantageClient(const std::string& api_key)
    : api_key_(api_key), base_url_("https://www.alphavantage.co/query") {
}

bool AlphaVantageClient::validate_api_key() {
    if (api_key_.empty()) {
        return false;
    }
    
    // Test with a simple query
    std::string url = base_url_ + "?function=TIME_SERIES_INTRADAY&symbol=IBM&interval=5min&apikey=" + api_key_;
    std::string response = APIUtils::http_get(url);
    
    // Check if response contains error message
    return (response.find("Invalid API call") == std::string::npos &&
            response.find("Error Message") == std::string::npos &&
            !response.empty());
}

std::string AlphaVantageClient::interval_to_string(DataInterval interval) {
    switch (interval) {
        case DataInterval::MINUTE_1: return "1min";
        case DataInterval::MINUTE_5: return "5min";
        case DataInterval::MINUTE_15: return "15min";
        case DataInterval::MINUTE_30: return "30min";
        case DataInterval::HOUR_1: return "60min";
        case DataInterval::DAILY: return "daily";
        case DataInterval::WEEKLY: return "weekly";
        case DataInterval::MONTHLY: return "monthly";
        default: return "daily";
    }
}

APIResponse AlphaVantageClient::fetch_historical_data(
    const std::string& symbol,
    DataInterval interval,
    const std::string& start_date,
    const std::string& end_date) {
    
    APIResponse response;
    response.success = false;
    
    // Validate dates
    if (!APIUtils::validate_date(start_date) || !APIUtils::validate_date(end_date)) {
        response.error_message = "Invalid date format. Use YYYY-MM-DD";
        return response;
    }
    
    // Build URL based on interval
    std::string url = base_url_ + "?apikey=" + api_key_ + "&symbol=" + symbol;
    
    if (interval == DataInterval::DAILY) {
        url += "&function=TIME_SERIES_DAILY&outputsize=full";
    } else if (interval == DataInterval::WEEKLY) {
        url += "&function=TIME_SERIES_WEEKLY";
    } else if (interval == DataInterval::MONTHLY) {
        url += "&function=TIME_SERIES_MONTHLY";
    } else {
        // Intraday
        url += "&function=TIME_SERIES_INTRADAY&interval=" + interval_to_string(interval) + "&outputsize=full";
    }
    
    std::cout << "Fetching data from Alpha Vantage: " << symbol << std::endl;
    std::string json_response = APIUtils::http_get(url);
    
    if (json_response.empty()) {
        response.error_message = "Failed to fetch data from API";
        return response;
    }
    
    // Check for API errors
    if (json_response.find("Error Message") != std::string::npos) {
        response.error_message = "API returned error - Invalid symbol or API limit reached";
        return response;
    }
    
    if (json_response.find("Note") != std::string::npos && 
        json_response.find("API call frequency") != std::string::npos) {
        response.error_message = "API rate limit exceeded. Please try again later.";
        return response;
    }
    
    // Parse response based on interval type
    if (interval == DataInterval::DAILY || interval == DataInterval::WEEKLY || interval == DataInterval::MONTHLY) {
        response = parse_daily_response(json_response);
    } else {
        response = parse_intraday_response(json_response);
    }
    
    // Filter by date range
    if (response.success && !response.data.empty()) {
        std::vector<MarketData> filtered_data;
        for (const auto& data : response.data) {
            std::string date_part = data.timestamp.substr(0, 10);
            if (date_part >= start_date && date_part <= end_date) {
                filtered_data.push_back(data);
            }
        }
        response.data = filtered_data;
    }
    
    return response;
}

APIResponse AlphaVantageClient::parse_daily_response(const std::string& json_response) {
    APIResponse response;
    response.success = false;
    
    // Simple JSON parsing (in production, use a proper JSON library)
    // Find the time series data
    size_t ts_pos = json_response.find("Time Series");
    if (ts_pos == std::string::npos) {
        response.error_message = "Invalid JSON response format";
        return response;
    }
    
    // Extract date-value pairs
    size_t pos = ts_pos;
    while (pos < json_response.length()) {
        // Find date (format: "YYYY-MM-DD")
        pos = json_response.find("\"", pos + 1);
        if (pos == std::string::npos) break;
        
        size_t date_start = pos + 1;
        pos = json_response.find("\"", date_start);
        if (pos == std::string::npos) break;
        
        std::string date = json_response.substr(date_start, pos - date_start);
        
        // Check if it's a valid date
        if (date.length() >= 10 && date[4] == '-' && date[7] == '-') {
            MarketData data;
            data.timestamp = date;
            
            // Extract OHLCV values
            size_t open_pos = json_response.find("\"1. open\":", pos);
            size_t high_pos = json_response.find("\"2. high\":", pos);
            size_t low_pos = json_response.find("\"3. low\":", pos);
            size_t close_pos = json_response.find("\"4. close\":", pos);
            size_t volume_pos = json_response.find("\"5. volume\":", pos);
            
            if (open_pos != std::string::npos && open_pos < pos + 500) {
                // Parse open
                size_t val_start = json_response.find("\"", open_pos + 10) + 1;
                size_t val_end = json_response.find("\"", val_start);
                data.open = std::stod(json_response.substr(val_start, val_end - val_start));
                
                // Parse high
                val_start = json_response.find("\"", high_pos + 10) + 1;
                val_end = json_response.find("\"", val_start);
                data.high = std::stod(json_response.substr(val_start, val_end - val_start));
                
                // Parse low
                val_start = json_response.find("\"", low_pos + 10) + 1;
                val_end = json_response.find("\"", val_start);
                data.low = std::stod(json_response.substr(val_start, val_end - val_start));
                
                // Parse close
                val_start = json_response.find("\"", close_pos + 10) + 1;
                val_end = json_response.find("\"", val_start);
                data.close = std::stod(json_response.substr(val_start, val_end - val_start));
                
                // Parse volume
                val_start = json_response.find("\"", volume_pos + 10) + 1;
                val_end = json_response.find("\"", val_start);
                data.volume = std::stod(json_response.substr(val_start, val_end - val_start));
                
                response.data.push_back(data);
            }
        }
        
        pos++;
    }
    
    if (!response.data.empty()) {
        response.success = true;
        // Reverse to get chronological order (oldest first)
        std::reverse(response.data.begin(), response.data.end());
    } else {
        response.error_message = "No data extracted from response";
    }
    
    return response;
}

APIResponse AlphaVantageClient::parse_intraday_response(const std::string& json_response) {
    // Similar to parse_daily_response but handles timestamp format
    return parse_daily_response(json_response);
}

APIResponse AlphaVantageClient::fetch_latest_quote(const std::string& symbol) {
    APIResponse response;
    response.success = false;
    
    std::string url = base_url_ + "?function=GLOBAL_QUOTE&symbol=" + symbol + "&apikey=" + api_key_;
    std::string json_response = APIUtils::http_get(url);
    
    if (json_response.empty()) {
        response.error_message = "Failed to fetch quote";
        return response;
    }
    
    // Parse quote (simplified)
    MarketData data;
    size_t price_pos = json_response.find("\"05. price\":");
    if (price_pos != std::string::npos) {
        size_t val_start = json_response.find("\"", price_pos + 12) + 1;
        size_t val_end = json_response.find("\"", val_start);
        data.close = std::stod(json_response.substr(val_start, val_end - val_start));
        data.open = data.high = data.low = data.close;
        data.volume = 0;
        
        // Get current timestamp
        time_t now = time(0);
        tm* ltm = localtime(&now);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", ltm);
        data.timestamp = buffer;
        
        response.data.push_back(data);
        response.success = true;
    } else {
        response.error_message = "Failed to parse quote";
    }
    
    return response;
}

// ============================================================================
// YahooFinanceClient Implementation
// ============================================================================

YahooFinanceClient::YahooFinanceClient()
    : base_url_("https://query1.finance.yahoo.com/v8/finance/chart/") {
    // Using v8 chart API which has better compatibility
}

long long YahooFinanceClient::date_to_timestamp(const std::string& date) {
    std::tm tm = {};
    std::istringstream ss(date);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    return static_cast<long long>(std::mktime(&tm));
}

APIResponse YahooFinanceClient::fetch_historical_data(
    const std::string& symbol,
    DataInterval interval,
    const std::string& start_date,
    const std::string& end_date) {
    
    APIResponse response;
    response.success = false;
    
    // Validate dates
    if (!APIUtils::validate_date(start_date) || !APIUtils::validate_date(end_date)) {
        response.error_message = "Invalid date format. Use YYYY-MM-DD";
        return response;
    }
    
    // Convert dates to timestamps
    long long period1 = date_to_timestamp(start_date);
    long long period2 = date_to_timestamp(end_date);
    
    // Calculate date range in days
    long long days_diff = (period2 - period1) / 86400; // 86400 seconds per day
    const long long MAX_DAYS_PER_REQUEST = 100; // Yahoo Finance limit
    
    // If date range is too long, split into chunks
    if (days_diff > MAX_DAYS_PER_REQUEST) {
        std::cout << "Date range too long (" << days_diff << " days), splitting into chunks..." << std::endl;
        
        APIResponse combined_response;
        combined_response.success = true;
        
        long long current_start = period1;
        int chunk_num = 0;
        while (current_start < period2) {
            long long current_end = current_start + (MAX_DAYS_PER_REQUEST * 86400);
            if (current_end > period2) current_end = period2;
            chunk_num++;
            
            // Convert timestamps back to date strings
            time_t t_start = static_cast<time_t>(current_start);
            time_t t_end = static_cast<time_t>(current_end);
            tm* tm_start = localtime(&t_start);
            tm* tm_end = localtime(&t_end);
            
            char chunk_start[20], chunk_end[20];
            strftime(chunk_start, sizeof(chunk_start), "%Y-%m-%d", tm_start);
            strftime(chunk_end, sizeof(chunk_end), "%Y-%m-%d", tm_end);
            
            std::cout << "Fetching chunk " << chunk_num << ": " << chunk_start << " to " << chunk_end 
                      << " (" << ((current_end - current_start) / 86400) << " days)" << std::endl;
            
            // Recursive call for this chunk (will not split again due to size)
            APIResponse chunk_response = fetch_historical_data(symbol, interval, chunk_start, chunk_end);
            
            if (chunk_response.success) {
                // Append data from this chunk
                combined_response.data.insert(
                    combined_response.data.end(),
                    chunk_response.data.begin(),
                    chunk_response.data.end()
                );
            } else {
                std::cerr << "Warning: Failed to fetch chunk " << chunk_start << " to " << chunk_end << std::endl;
            }
            
            current_start = current_end;
        }
        
        if (!combined_response.data.empty()) {
            std::cout << "Successfully fetched " << combined_response.data.size() << " total data points" << std::endl;
            return combined_response;
        } else {
            response.error_message = "Failed to fetch any data chunks";
            return response;
        }
    }
    
    // Normal single request for date ranges <= 100 days
    // Build URL for v8 chart API
    std::string interval_str = "1d"; // Default to daily
    if (interval == DataInterval::WEEKLY) interval_str = "1wk";
    else if (interval == DataInterval::MONTHLY) interval_str = "1mo";
    
    // v8 chart API format - simpler and no auth required
    std::string url = base_url_ + symbol + "?period1=" + std::to_string(period1) +
                      "&period2=" + std::to_string(period2) + "&interval=" + interval_str;
    
    std::cout << "Fetching data from Yahoo Finance: " << symbol << std::endl;
    std::string csv_response = APIUtils::http_get(url);
    
    std::cout << "Response length: " << csv_response.length() << " bytes" << std::endl;
    
    if (csv_response.empty()) {
        response.error_message = "Failed to fetch data from Yahoo Finance - Empty response";
        return response;
    }
    
    // Check if response is JSON (v8 API) or CSV (v7 API)
    if (csv_response.find("{\"chart\"") != std::string::npos) {
        // v8 chart API response (JSON format)
        response = parse_json_chart_response(csv_response);
    } else {
        // v7 download API response (CSV format)
        // Check for error responses
        if (csv_response.find("404") != std::string::npos || 
            csv_response.find("Not Found") != std::string::npos) {
            response.error_message = "Symbol not found or API endpoint changed";
            return response;
        }
        response = parse_csv_response(csv_response);
    }
    return response;
}

APIResponse YahooFinanceClient::parse_csv_response(const std::string& csv_response) {
    APIResponse response;
    response.success = false;
    
    std::istringstream stream(csv_response);
    std::string line;
    
    // Skip header
    std::getline(stream, line);
    std::cout << "CSV Header: " << line << std::endl;
    
    int line_count = 0;
    int valid_lines = 0;
    
    while (std::getline(stream, line)) {
        line_count++;
        if (line.empty()) continue;
        
        // Remove any carriage returns
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        
        std::istringstream line_stream(line);
        std::string date, open, high, low, close, adj_close, volume;
        
        std::getline(line_stream, date, ',');
        std::getline(line_stream, open, ',');
        std::getline(line_stream, high, ',');
        std::getline(line_stream, low, ',');
        std::getline(line_stream, close, ',');
        std::getline(line_stream, adj_close, ',');
        std::getline(line_stream, volume, ',');
        
        // Skip lines with "null" values
        if (open == "null" || high == "null" || low == "null" || 
            close == "null" || volume == "null") {
            continue;
        }
        
        try {
            MarketData data;
            data.timestamp = date;
            data.open = std::stod(open);
            data.high = std::stod(high);
            data.low = std::stod(low);
            data.close = std::stod(close);
            data.volume = std::stod(volume);
            
            response.data.push_back(data);
            valid_lines++;
        } catch (const std::exception& e) {
            std::cerr << "Failed to parse line " << line_count << ": " << line << std::endl;
            std::cerr << "Error: " << e.what() << std::endl;
            continue;
        }
    }
    
    std::cout << "Parsed " << valid_lines << " valid lines out of " << line_count << " total lines" << std::endl;
    
    if (!response.data.empty()) {
        response.success = true;
    } else {
        response.error_message = "No valid data in response (parsed " + 
                                std::to_string(line_count) + " lines, 0 valid)";
    }
    
    return response;
}

APIResponse YahooFinanceClient::parse_json_chart_response(const std::string& json_response) {
    APIResponse response;
    response.success = false;
    
    // Simple JSON parsing for v8 chart API
    // Find the timestamps array
    size_t timestamp_pos = json_response.find("\"timestamp\":[");
    if (timestamp_pos == std::string::npos) {
        response.error_message = "Could not find timestamp data in response";
        return response;
    }
    
    // Find indicators (open, high, low, close, volume)
    size_t open_pos = json_response.find("\"open\":[", timestamp_pos);
    size_t high_pos = json_response.find("\"high\":[", timestamp_pos);
    size_t low_pos = json_response.find("\"low\":[", timestamp_pos);
    size_t close_pos = json_response.find("\"close\":[", timestamp_pos);
    size_t volume_pos = json_response.find("\"volume\":[", timestamp_pos);
    
    if (open_pos == std::string::npos || close_pos == std::string::npos) {
        response.error_message = "Could not find price data in response";
        return response;
    }
    
    // Extract timestamp array
    size_t ts_start = timestamp_pos + 13;
    size_t ts_end = json_response.find("]", ts_start);
    std::string timestamps_str = json_response.substr(ts_start, ts_end - ts_start);
    
    // Extract price arrays
    auto extract_array = [&json_response](size_t pos) -> std::vector<double> {
        std::vector<double> result;
        if (pos == std::string::npos) return result;
        
        size_t start = json_response.find("[", pos) + 1;
        size_t end = json_response.find("]", start);
        std::string arr_str = json_response.substr(start, end - start);
        
        std::istringstream iss(arr_str);
        std::string val;
        while (std::getline(iss, val, ',')) {
            try {
                if (val.find("null") == std::string::npos && !val.empty()) {
                    result.push_back(std::stod(val));
                } else {
                    result.push_back(0.0); // null values
                }
            } catch (...) {
                result.push_back(0.0);
            }
        }
        return result;
    };
    
    std::vector<double> opens = extract_array(open_pos);
    std::vector<double> highs = extract_array(high_pos);
    std::vector<double> lows = extract_array(low_pos);
    std::vector<double> closes = extract_array(close_pos);
    std::vector<double> volumes = extract_array(volume_pos);
    
    // Parse timestamps
    std::vector<long long> timestamps;
    std::istringstream ts_stream(timestamps_str);
    std::string ts_val;
    while (std::getline(ts_stream, ts_val, ',')) {
        try {
            timestamps.push_back(std::stoll(ts_val));
        } catch (...) {
            continue;
        }
    }
    
    // Combine into MarketData
    size_t count = timestamps.size();
    if (opens.size() < count) count = opens.size();
    if (closes.size() < count) count = closes.size();
    
    for (size_t i = 0; i < count; ++i) {
        if (closes[i] > 0.0) { // Skip null/zero values
            MarketData data;
            
            // Convert timestamp to date string
            time_t t = static_cast<time_t>(timestamps[i]);
            tm* ltm = localtime(&t);
            char date_str[20];
            strftime(date_str, sizeof(date_str), "%Y-%m-%d", ltm);
            data.timestamp = date_str;
            
            data.open = (i < opens.size()) ? opens[i] : closes[i];
            data.high = (i < highs.size()) ? highs[i] : closes[i];
            data.low = (i < lows.size()) ? lows[i] : closes[i];
            data.close = closes[i];
            data.volume = (i < volumes.size()) ? volumes[i] : 0.0;
            
            response.data.push_back(data);
        }
    }
    
    if (!response.data.empty()) {
        response.success = true;
        std::cout << "Successfully parsed " << response.data.size() << " data points from JSON" << std::endl;
    } else {
        response.error_message = "No valid data found in JSON response";
    }
    
    return response;
}

APIResponse YahooFinanceClient::fetch_latest_quote(const std::string& symbol) {
    // Get last day's data
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char end_date[11];
    strftime(end_date, sizeof(end_date), "%Y-%m-%d", ltm);
    
    time_t yesterday = now - 86400;
    tm* ytm = localtime(&yesterday);
    char start_date[11];
    strftime(start_date, sizeof(start_date), "%Y-%m-%d", ytm);
    
    return fetch_historical_data(symbol, DataInterval::DAILY, start_date, end_date);
}

// ============================================================================
// APIDataFetcher Implementation
// ============================================================================

APIDataFetcher::APIDataFetcher()
    : active_provider_(APIProvider::ALPHA_VANTAGE), caching_enabled_(true) {
}

bool APIDataFetcher::initialize(const std::map<std::string, std::string>& config) {
    try {
        // Initialize Alpha Vantage if API key is provided
        auto av_key = config.find("alpha_vantage_key");
        if (av_key != config.end() && !av_key->second.empty()) {
            clients_[APIProvider::ALPHA_VANTAGE] = 
                std::make_unique<AlphaVantageClient>(av_key->second);
            std::cout << "Alpha Vantage client initialized" << std::endl;
        }
        
        // Initialize Yahoo Finance (no API key needed)
        clients_[APIProvider::YAHOO_FINANCE] = 
            std::make_unique<YahooFinanceClient>();
        std::cout << "Yahoo Finance client initialized" << std::endl;
        
        // Set default provider
        if (clients_.find(active_provider_) == clients_.end()) {
            active_provider_ = APIProvider::YAHOO_FINANCE;
        }
        
        return !clients_.empty();
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize API clients: " << e.what() << std::endl;
        return false;
    }
}

bool APIDataFetcher::set_provider(APIProvider provider) {
    if (clients_.find(provider) != clients_.end()) {
        active_provider_ = provider;
        return true;
    }
    return false;
}

APIResponse APIDataFetcher::fetch_data(
    const std::string& symbol,
    DataInterval interval,
    const std::string& start_date,
    const std::string& end_date) {
    
    // Check cache first
    if (caching_enabled_ && is_cached(symbol, interval)) {
        std::cout << "Using cached data for " << symbol << std::endl;
        return get_cached_data(symbol, interval);
    }
    
    // Fetch from active provider
    auto it = clients_.find(active_provider_);
    if (it == clients_.end()) {
        APIResponse response;
        response.success = false;
        response.error_message = "No active API provider";
        return response;
    }
    
    APIResponse response = it->second->fetch_historical_data(symbol, interval, start_date, end_date);
    
    // Cache successful response
    if (response.success && caching_enabled_) {
        cache_data(symbol, interval, response);
    }
    
    return response;
}

APIResponse APIDataFetcher::fetch_quote(const std::string& symbol) {
    auto it = clients_.find(active_provider_);
    if (it == clients_.end()) {
        APIResponse response;
        response.success = false;
        response.error_message = "No active API provider";
        return response;
    }
    
    return it->second->fetch_latest_quote(symbol);
}

bool APIDataFetcher::save_to_csv(const APIResponse& response, const std::string& filename) {
    if (!response.success || response.data.empty()) {
        return false;
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Write header
    file << "timestamp,open,high,low,close,volume\n";
    
    // Write data
    for (const auto& data : response.data) {
        file << data.timestamp << ","
             << std::fixed << std::setprecision(2)
             << data.open << ","
             << data.high << ","
             << data.low << ","
             << data.close << ","
             << std::fixed << std::setprecision(0)
             << data.volume << "\n";
    }
    
    file.close();
    return true;
}

std::vector<std::string> APIDataFetcher::get_available_providers() const {
    std::vector<std::string> providers;
    for (const auto& pair : clients_) {
        providers.push_back(pair.second->get_provider_name());
    }
    return providers;
}

void APIDataFetcher::clear_cache() {
    cache_.clear();
}

bool APIDataFetcher::is_cached(const std::string& symbol, DataInterval interval) const {
    auto symbol_it = cache_.find(symbol);
    if (symbol_it != cache_.end()) {
        return symbol_it->second.find(interval) != symbol_it->second.end();
    }
    return false;
}

APIResponse APIDataFetcher::get_cached_data(const std::string& symbol, DataInterval interval) const {
    return cache_.at(symbol).at(interval);
}

void APIDataFetcher::cache_data(const std::string& symbol, DataInterval interval, const APIResponse& response) {
    cache_[symbol][interval] = response;
}

} // namespace TradingBot

