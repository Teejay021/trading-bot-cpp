# AI-Driven Trading Bot

A clean, modular C++ trading bot designed for backtesting strategies on historical data. Built with a focus on learning and safety. Start offline with CSV data before considering live trading.

## Project Structure

```
tradingBot/
├── include/                 # Header files
│   ├── data/               # Data handling (CSV parsing)
│   ├── strategy/           # Trading strategies
│   ├── risk/               # Risk management
│   ├── backtester/         # Backtesting engine
│   ├── reporting/          # Report generation
│   └── utils/              # Utilities (logging, etc.)
├── src/                    # Source files
│   ├── data/               # CSV parser implementation
│   ├── strategy/           # Strategy implementations
│   ├── risk/               # Risk manager implementation
│   ├── backtester/         # Backtester implementation
│   ├── reporting/          # Report generator implementation
│   └── utils/              # Logger implementation
├── tests/                  # Unit tests
├── data/                   # Sample data files
├── reports/                # Generated reports
├── logs/                   # Log files
├── CMakeLists.txt          # Main build configuration
└── README.md               # This file
```

## Features

- **Modular Design**: Clean separation of concerns for easy testing and modification
- **CSV Data Support**: Parse and validate market data from CSV files
- **🆕 API Data Integration**: Fetch real-time market data from Yahoo Finance and Alpha Vantage
- **Strategy Framework**: Extensible strategy system with base class inheritance
- **Multiple Strategies**: SMA Crossover, EMA Crossover, RSI strategies included
- **Risk Management**: Position sizing, stop-loss, drawdown protection
- **Backtesting Engine**: Historical strategy simulation with realistic costs
- **Comprehensive Reporting**: Multiple output formats (CSV, HTML, JSON)
- **Logging System**: Detailed logging for debugging and monitoring
- **Data Caching**: Built-in caching to minimize API calls

## Getting Started

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.16+
- Git
- Internet connection (for API data fetching)

### Building the Project

```bash
# Clone the repository
git clone https://github.com/Teejay021/trading-bot-cpp
cd tradingBot

# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
cmake --build . --config Release

# Run tests (optional)
ctest --output-on-failure
```

### Running the Bot

#### Option 1: With API Data (Recommended - No CSV needed!)
```bash
# Test API data fetching
cd build/bin/Release
./test_api_data_fetcher.exe

# Run backtest with live data
./test_trading_bot_with_api.exe
```

#### Option 2: With CSV Files
```bash
# Basic usage
./bin/trading_bot <data_file> <strategy_name>

# Example
./bin/trading_bot ../data/SPY.csv SMA_CROSSOVER
```

#### Quick Start Guide
See `QUICKSTART_API.md` for step-by-step instructions on using the API features.

## Data Format

The bot expects CSV files with the following columns:
- `timestamp`: Date/time in ISO format (YYYY-MM-DD HH:MM:SS)
- `open`: Opening price
- `high`: High price
- `low`: Low price
- `close`: Closing price
- `volume`: Trading volume

Example:
```csv
timestamp,open,high,low,close,volume
2023-01-01 09:30:00,100.00,101.50,99.50,101.00,1000000
2023-01-01 09:31:00,101.00,102.00,100.75,101.75,950000
```

## Strategies

### SMA Crossover Strategy
A simple moving average crossover strategy that:
- Buys when short-term SMA crosses above long-term SMA
- Sells when short-term SMA crosses below long-term SMA
- Configurable periods for short and long moving averages

### EMA Crossover Strategy
Exponential moving average crossover:
- Uses EMA instead of SMA for faster reaction to price changes
- Better for trending markets
- Configurable short and long periods

### RSI Strategy
Relative Strength Index momentum strategy:
- Buys when RSI crosses below oversold threshold (default 30)
- Sells when RSI crosses above overbought threshold (default 70)
- Configurable period and thresholds

### Adding New Strategies
1. Inherit from the `Strategy` base class
2. Implement required virtual methods
3. Add strategy to the strategy factory
4. Update configuration files

## Risk Management

The bot includes several risk management features:
- **Position Sizing**: Based on portfolio percentage and ATR
- **Stop Loss**: Configurable percentage-based stop losses
- **Take Profit**: Configurable profit targets
- **Drawdown Protection**: Maximum drawdown limits
- **Daily Loss Limits**: Maximum daily loss protection

## Configuration

Create a `config.json` file to customize:
- Initial capital
- Commission rates
- Risk parameters
- Strategy parameters
- Logging levels

## Testing

Run the test suite to ensure everything works correctly:
```bash
cd build
ctest --output-on-failure
```

## 🆕 New: API Data Integration

Fetch real market data directly from APIs:

```cpp
#include "trading_bot.h"

int main() {
    TradingBot::TradingBot bot;
    bot.initialize("config.json");
    
    // Run backtest with API data (no CSV needed!)
    bot.run_backtest_with_api(
        "AAPL",           // Symbol
        "SMA_CROSSOVER",  // Strategy  
        "2024-01-01",     // Start date
        "2024-10-07"      // End date
    );
    
    bot.generate_report("AAPL_report.html");
    return 0;
}
```

**Supported Providers:**
- **Yahoo Finance** (default, no API key required)
- **Alpha Vantage** (optional, for intraday data)

**See `QUICKSTART_API.md` and `API_INTEGRATION_GUIDE.md` for details.**

## Future Enhancements

- **Machine Learning Integration**: Add ML-based signal generation
- **Paper Trading**: Simulate live trading without real money
- ~~**Real-time Data**: Connect to live market data feeds~~ ✅ **DONE!**
- **Portfolio Optimization**: Multi-asset portfolio management
- **Advanced Analytics**: More sophisticated performance metrics

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests for new functionality
5. Submit a pull request

## License

This project is for educational purposes. Use at your own risk.

## Disclaimer

This software is for educational and research purposes only. It is not financial advice. Trading involves risk and you can lose money. Always test thoroughly before using with real money.
