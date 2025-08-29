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

# Main executable
add_executable(trading_bot 
    src/main.cpp
    src/backtester/backtester.cpp
    src/data/csv_parser.cpp
    src/strategy/strategy.cpp
    src/risk/risk_manager.cpp
    src/reporting/report_generator.cpp
    src/utils/logger.cpp
)

# Include directories
target_include_directories(trading_bot PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(trading_bot PRIVATE
    csv_parser
    strategy
    risk_manager
    report_generator
    logger
)

# Compiler flags
if(MSVC)
    target_compile_options(trading_bot PRIVATE /W4)
else()
    target_compile_options(trading_bot PRIVATE -Wall -Wextra -Wpedantic)
endif()
