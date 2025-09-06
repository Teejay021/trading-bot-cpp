# Only build libraries that exist

# CSV Parser library
add_library(csv_parser
    data/csv_parser.cpp
)

target_include_directories(csv_parser PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Strategy library
add_library(strategy
    strategy/strategy.cpp
    strategy/sma_crossover_strategy.cpp
    strategy/rsi_strategy.cpp
)

target_include_directories(strategy PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(strategy PUBLIC csv_parser)

# Commented out libraries that don't exist yet
# # Risk Manager library
# add_library(risk_manager
#     risk/risk_manager.cpp
# )
# 
# target_include_directories(risk_manager PUBLIC
#     ${CMAKE_SOURCE_DIR}/include
# )
# 
# target_link_libraries(risk_manager PUBLIC strategy)
# 
# # Backtester library
# add_library(backtester
#     backtester/backtester.cpp
# )
# 
# target_include_directories(backtester PUBLIC
#     ${CMAKE_SOURCE_DIR}/include
# )
# 
# target_link_libraries(backtester PUBLIC risk_manager)
# 
# # Report Generator library
# add_library(report_generator
#     reporting/report_generator.cpp
# )
# 
# target_include_directories(report_generator PUBLIC
#     ${CMAKE_SOURCE_DIR}/include
# )
# 
# target_link_libraries(report_generator PUBLIC backtester)
# 
# # Logger library
# add_library(logger
#     utils/logger.cpp
# )
# 
# target_include_directories(logger PUBLIC
#     ${CMAKE_SOURCE_DIR}/include
# )
