# Trading Bot Example Configurations

This directory contains pre-configured strategy examples to help you get started with backtesting different trading approaches.

## Available Configurations

### 1. Aggressive SMA Strategy (`config_aggressive_sma.json`)

**Profile:** High-frequency trading with quick entries/exits
- **Strategy:** Fast SMA crossover (5/20 periods)
- **Target Asset:** TSLA (high volatility stock)
- **Capital:** $50,000
- **Risk Level:** High
- **Features:**
  - Short selling enabled
  - Larger position sizes (5% max)
  - Tighter stops (3%) with larger profit targets (15%)
  - Higher max drawdown tolerance (30%)

**Best for:** Traders comfortable with high risk/reward ratios and volatile markets

---

### 2. Conservative SMA Strategy (`config_conservative_sma.json`)

**Profile:** Long-term trend following with capital preservation
- **Strategy:** Classic Golden Cross/Death Cross (50/200 periods)
- **Target Asset:** SPY (S&P 500 ETF)
- **Capital:** $100,000
- **Risk Level:** Low
- **Features:**
  - Long-only positions
  - Small position sizes (1% max)
  - Tight risk controls (15% max drawdown)
  - Lower stops (2%) with modest profit targets (8%)

**Best for:** Conservative investors seeking steady, long-term returns

---

### 3. RSI Scalping Strategy (`config_rsi_scalping.json`)

**Profile:** Mean-reversion scalping based on momentum
- **Strategy:** RSI overbought/oversold (30/70 thresholds)
- **Target Asset:** NVDA (technology stock)
- **Capital:** $25,000
- **Risk Level:** Medium-High
- **Features:**
  - Short selling enabled
  - Moderate position sizes (3% max)
  - Very tight stops (1.5%) for quick exits
  - Short-term focus (2022-2023)
  - Lower commission rates for frequent trading

**Best for:** Active traders looking for short-term opportunities

---

### 4. EMA Swing Trading Strategy (`config_ema_swing.json`)

**Profile:** Medium-term trend trading using exponential moving averages
- **Strategy:** EMA crossover (12/26/9 periods, MACD-like)
- **Target Asset:** AAPL (stable blue-chip stock)
- **Capital:** $75,000
- **Risk Level:** Medium
- **Features:**
  - Long-only positions
  - Moderate position sizes (2.5% max)
  - Balanced stops (4%) and targets (12%)
  - Medium-term holding periods

**Best for:** Swing traders seeking balance between risk and opportunity

---

## How to Use

### Option 1: Command Line
```bash
# Using the compiled executable
./build/bin/Release/TradingBot examples/config_aggressive_sma.json

# Or for a different strategy
./build/bin/Release/TradingBot examples/config_conservative_sma.json
```

### Option 2: Run Script (if available)
```bash
# Run a specific configuration
python run_backtest.py --config examples/config_rsi_scalping.json

# Compare multiple strategies
python run_backtest.py --compare examples/*.json
```

### Option 3: Copy and Customize
```bash
# Copy a template to your project root
cp examples/config_ema_swing.json my_custom_config.json

# Edit with your preferences
nano my_custom_config.json

# Run your custom configuration
./build/bin/Release/TradingBot my_custom_config.json
```

---

## Configuration Parameters Explained

### Risk Management Settings

| Parameter | Description | Conservative | Moderate | Aggressive |
|-----------|-------------|--------------|----------|------------|
| `max_position_size` | Maximum % of capital per trade | 1-2% | 2-3% | 5%+ |
| `max_drawdown` | Maximum portfolio loss threshold | 10-15% | 15-25% | 25-35% |
| `stop_loss_pct` | Stop loss distance from entry | 1-2% | 2-4% | 3-5% |
| `take_profit_pct` | Take profit target | 5-8% | 8-12% | 12-20% |
| `max_daily_loss` | Maximum loss allowed per day | 2-3% | 3-6% | 6-10% |

### Strategy Periods

**SMA/EMA Short Periods:**
- Fast (scalping): 5-10
- Medium (day/swing): 10-20
- Slow (position): 20-50

**SMA/EMA Long Periods:**
- Fast (scalping): 20-30
- Medium (day/swing): 30-100
- Slow (position): 100-200

**RSI Thresholds:**
- Aggressive: 25/75 (more signals)
- Standard: 30/70 (balanced)
- Conservative: 35/65 (fewer signals)

---

## Backtesting Tips

1. **Start Conservative**: Begin with `config_conservative_sma.json` to understand baseline performance
2. **Compare Strategies**: Run multiple configs on the same asset/timeframe to compare
3. **Adjust for Market**: High volatility markets favor RSI/fast SMA, trending markets favor slow SMA/EMA
4. **Mind the Costs**: Lower commission rates favor scalping, higher rates favor swing trading
5. **Validate Results**: Always backtest on historical data before live trading

---

## Customization Ideas

### Modify for Different Assets
```json
"api": {
    "default_symbol": "BTC-USD",  // Cryptocurrency
    "default_interval": "hourly"   // Higher frequency data
}
```

### Add Multiple Strategies
```json
"strategies": {
    "SMA_CROSSOVER": { "short_period": 10, "long_period": 30 },
    "RSI": { "period": 14, "oversold_threshold": 30 }
}
```

### Adjust Time Periods
```json
"backtesting": {
    "start_date": "2023-01-01",  // Recent market
    "end_date": "2024-01-01"
}
```

---

## Performance Metrics to Monitor

After running a backtest, review your HTML report for:
- **Total Return %**: Overall profit/loss percentage
- **Sharpe Ratio**: Risk-adjusted returns (higher is better)
- **Max Drawdown**: Worst peak-to-trough decline
- **Win Rate**: Percentage of profitable trades
- **Profit Factor**: Gross profit / gross loss (>1.5 is good)
- **Average Trade Duration**: How long positions are held

---

## Disclaimer

⚠️ **Important**: These configurations are for educational and backtesting purposes only. Past performance does not guarantee future results. Always:
- Test thoroughly before live trading
- Start with paper trading
- Never risk more than you can afford to lose
- Consult with financial advisors for investment decisions

---

## Contributing

Have a profitable strategy configuration? Feel free to submit a pull request with:
- Configuration file
- Description of the strategy
- Backtest results/screenshots
- Recommended use cases

Happy Trading! 📈🤖



