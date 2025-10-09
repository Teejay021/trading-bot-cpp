cmake_minimum_required(VERSION 3.16)
project(TradingBot VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find required packages
# find_package(PkgConfig REQUIRED)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(tests)

# Main executable (commented out until all components are ready)
# add_executable(trading_bot 
#     src/main.cpp
#     src/backtester/backtester.cpp
#     src/data/csv_parser.cpp
#     src/strategy/strategy.cpp
#     src/risk/risk_manager.cpp
#     src/reporting/report_generator.cpp
#     src/utils/logger.cpp
# )

# Test executable for CSV parser
add_executable(test_csv
    test_csv.cpp
    src/data/csv_parser.cpp
)

# Test executable for SMA strategy
add_executable(test_sma_strategy
    test_sma_strategy.cpp
    src/data/csv_parser.cpp
    src/strategy/strategy.cpp
    src/strategy/sma_crossover_strategy.cpp
)

# Include directories (commented out until main executable is ready)
# target_include_directories(trading_bot PRIVATE
#     ${CMAKE_SOURCE_DIR}/include
#     ${CMAKE_SOURCE_DIR}/src
# )

target_include_directories(test_csv PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_include_directories(test_sma_strategy PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Test executable for RSI strategy
add_executable(test_rsi_strategy
    test_rsi_strategy.cpp
    src/data/csv_parser.cpp
    src/strategy/strategy.cpp
    src/strategy/rsi_strategy.cpp
)

# Test executable for EMA strategy
add_executable(test_ema_strategy
    test_ema_strategy.cpp
    src/data/csv_parser.cpp
    src/strategy/strategy.cpp
    src/strategy/ema_strategy.cpp
)

target_include_directories(test_rsi_strategy PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_include_directories(test_ema_strategy PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Test executable for Risk Manager
add_executable(test_risk_manager
    test_risk_manager.cpp
    src/data/csv_parser.cpp
    src/strategy/strategy.cpp
    src/risk/risk_manager.cpp
)

target_include_directories(test_risk_manager PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Test executable for Backtester
add_executable(test_backtester
    test_backtester.cpp
    src/data/csv_parser.cpp
    src/strategy/strategy.cpp
    src/strategy/sma_crossover_strategy.cpp
    src/risk/risk_manager.cpp
    src/backtester/backtester.cpp
)

target_include_directories(test_backtester PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Test executable for complete TradingBot integration
add_executable(test_trading_bot
    test_trading_bot.cpp
    src/data/csv_parser.cpp
    src/strategy/strategy.cpp
    src/strategy/sma_crossover_strategy.cpp
    src/strategy/ema_strategy.cpp
    src/strategy/rsi_strategy.cpp
    src/risk/risk_manager.cpp
    src/backtester/backtester.cpp
    src/trading_bot.cpp
)

target_include_directories(test_trading_bot PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Simple test executable for TradingBot
add_executable(test_simple_trading_bot
    test_simple_trading_bot.cpp
    src/data/csv_parser.cpp
    src/strategy/strategy.cpp
    src/strategy/sma_crossover_strategy.cpp
    src/strategy/ema_strategy.cpp
    src/strategy/rsi_strategy.cpp
    src/risk/risk_manager.cpp
    src/backtester/backtester.cpp
    src/trading_bot.cpp
    src/utils/logger.cpp
    src/reporting/report_generator.cpp
)

target_include_directories(test_simple_trading_bot PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Complete system test executable
add_executable(test_complete_system
    test_complete_system.cpp
    src/data/csv_parser.cpp
    src/strategy/strategy.cpp
    src/strategy/sma_crossover_strategy.cpp
    src/strategy/ema_strategy.cpp
    src/strategy/rsi_strategy.cpp
    src/risk/risk_manager.cpp
    src/backtester/backtester.cpp
    src/trading_bot.cpp
    src/utils/logger.cpp
    src/reporting/report_generator.cpp
)

target_include_directories(test_complete_system PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Test executable for API Data Fetcher
add_executable(test_api_data_fetcher
    test_api_data_fetcher.cpp
    src/data/api_data_fetcher.cpp
)

target_include_directories(test_api_data_fetcher PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Link WinHTTP on Windows, or libcurl on Unix-like systems
if(WIN32)
    target_link_libraries(test_api_data_fetcher PRIVATE winhttp)
else()
    find_package(CURL REQUIRED)
    target_include_directories(test_api_data_fetcher PRIVATE ${CURL_INCLUDE_DIR})
    target_link_libraries(test_api_data_fetcher PRIVATE ${CURL_LIBRARIES})
endif()

# Test executable for TradingBot with API integration
add_executable(test_trading_bot_with_api
    test_trading_bot_with_api.cpp
    src/data/csv_parser.cpp
    src/data/api_data_fetcher.cpp
    src/strategy/strategy.cpp
    src/strategy/sma_crossover_strategy.cpp
    src/strategy/ema_strategy.cpp
    src/strategy/rsi_strategy.cpp
    src/risk/risk_manager.cpp
    src/backtester/backtester.cpp
    src/trading_bot.cpp
    src/utils/logger.cpp
    src/reporting/report_generator.cpp
)

target_include_directories(test_trading_bot_with_api PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Link WinHTTP on Windows, or libcurl on Unix-like systems
if(WIN32)
    target_link_libraries(test_trading_bot_with_api PRIVATE winhttp)
else()
    find_package(CURL REQUIRED)
    target_include_directories(test_trading_bot_with_api PRIVATE ${CURL_INCLUDE_DIR})
    target_link_libraries(test_trading_bot_with_api PRIVATE ${CURL_LIBRARIES})
endif()

# Link libraries (commented out until main executable is ready)
# target_link_libraries(trading_bot PRIVATE
#     csv_parser
#     strategy
#     risk_manager
#     report_generator
#     logger
# )

# Compiler flags (commented out until main executable is ready)
# if(MSVC)
#     target_compile_options(trading_bot PRIVATE /W4)
# else()
#     target_compile_options(trading_bot PRIVATE -Wall -Wextra -Wpedantic)
# endif()
