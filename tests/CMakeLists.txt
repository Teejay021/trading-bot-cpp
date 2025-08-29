# Enable testing
enable_testing()

# Find Google Test (you'll need to install this)
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    message(WARNING "Google Test not found. Tests will not be built.")
    return()
endif()

# Test executable
add_executable(trading_bot_tests
    test_csv_parser.cpp
    test_strategy.cpp
    test_risk_manager.cpp
    test_backtester.cpp
    test_report_generator.cpp
    test_logger.cpp
)

# Include directories
target_include_directories(trading_bot_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(trading_bot_tests PRIVATE
    csv_parser
    strategy
    risk_manager
    backtester
    report_generator
    logger
    GTest::GTest
    GTest::Main
)

# Add tests
add_test(NAME CSV_Parser_Tests COMMAND trading_bot_tests --gtest_filter="CSV_Parser*")
add_test(NAME Strategy_Tests COMMAND trading_bot_tests --gtest_filter="Strategy*")
add_test(NAME Risk_Manager_Tests COMMAND trading_bot_tests --gtest_filter="RiskManager*")
add_test(NAME Backtester_Tests COMMAND trading_bot_tests --gtest_filter="Backtester*")
add_test(NAME Report_Generator_Tests COMMAND trading_bot_tests --gtest_filter="ReportGenerator*")
add_test(NAME Logger_Tests COMMAND trading_bot_tests --gtest_filter="Logger*")
add_test(NAME All_Tests COMMAND trading_bot_tests)
