#!/usr/bin/env python3
"""
Helper script to run Trading Bot examples easily
"""

import subprocess
import sys
import os
import argparse
from pathlib import Path

# Define available examples
EXAMPLES = {
    "aggressive": {
        "config": "examples/config_aggressive_sma.json",
        "description": "Aggressive SMA strategy on TSLA (High Risk)"
    },
    "conservative": {
        "config": "examples/config_conservative_sma.json",
        "description": "Conservative SMA strategy on SPY (Low Risk)"
    },
    "rsi": {
        "config": "examples/config_rsi_scalping.json",
        "description": "RSI scalping strategy on NVDA (Medium-High Risk)"
    },
    "ema": {
        "config": "examples/config_ema_swing.json",
        "description": "EMA swing trading strategy on AAPL (Medium Risk)"
    }
}

def find_executable():
    """Find the TradingBot executable"""
    possible_paths = [
        "build/bin/Release/TradingBot.exe",
        "build/bin/Debug/TradingBot.exe",
        "build/bin/TradingBot.exe",
        "build/bin/Release/TradingBot",
        "build/TradingBot.exe",
        "TradingBot.exe",
    ]
    
    for path in possible_paths:
        if os.path.exists(path):
            return path
    
    return None

def list_examples():
    """List all available examples"""
    print("\n" + "="*60)
    print("Available Trading Bot Examples".center(60))
    print("="*60 + "\n")
    
    for name, info in EXAMPLES.items():
        print(f"  {name:15} - {info['description']}")
    
    print("\n" + "="*60 + "\n")

def run_example(example_name, executable_path=None):
    """Run a specific example"""
    if example_name not in EXAMPLES:
        print(f"❌ Error: Unknown example '{example_name}'")
        print(f"Available examples: {', '.join(EXAMPLES.keys())}")
        return 1
    
    config_path = EXAMPLES[example_name]["config"]
    
    if not os.path.exists(config_path):
        print(f"❌ Error: Configuration file not found: {config_path}")
        return 1
    
    # Find executable if not provided
    if executable_path is None:
        executable_path = find_executable()
    
    if executable_path is None:
        print("❌ Error: TradingBot executable not found!")
        print("Please build the project first using:")
        print("  cmake -B build -DCMAKE_BUILD_TYPE=Release")
        print("  cmake --build build --config Release")
        return 1
    
    if not os.path.exists(executable_path):
        print(f"❌ Error: Executable not found at: {executable_path}")
        return 1
    
    print("\n" + "="*60)
    print(f"Running: {EXAMPLES[example_name]['description']}")
    print(f"Config: {config_path}")
    print(f"Executable: {executable_path}")
    print("="*60 + "\n")
    
    # Run the trading bot
    try:
        result = subprocess.run(
            [executable_path, config_path],
            check=True,
            capture_output=False
        )
        
        print("\n" + "="*60)
        print("✅ Backtest completed successfully!")
        print("="*60 + "\n")
        return 0
        
    except subprocess.CalledProcessError as e:
        print(f"\n❌ Error: Backtest failed with exit code {e.returncode}")
        return e.returncode
    except KeyboardInterrupt:
        print("\n\n⚠️  Backtest interrupted by user")
        return 130
    except Exception as e:
        print(f"\n❌ Error: {e}")
        return 1

def main():
    parser = argparse.ArgumentParser(
        description="Run Trading Bot example configurations",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python run_example.py --list                    # List all examples
  python run_example.py aggressive                # Run aggressive SMA strategy
  python run_example.py conservative              # Run conservative SMA strategy
  python run_example.py rsi                       # Run RSI scalping strategy
  python run_example.py ema                       # Run EMA swing strategy
  python run_example.py aggressive --exe ./build/bin/Release/TradingBot.exe
        """
    )
    
    parser.add_argument(
        "example",
        nargs="?",
        choices=list(EXAMPLES.keys()),
        help="Example to run"
    )
    
    parser.add_argument(
        "--list", "-l",
        action="store_true",
        help="List all available examples"
    )
    
    parser.add_argument(
        "--exe", "-e",
        type=str,
        help="Path to TradingBot executable (auto-detected if not provided)"
    )
    
    args = parser.parse_args()
    
    if args.list or args.example is None:
        list_examples()
        return 0
    
    return run_example(args.example, args.exe)

if __name__ == "__main__":
    sys.exit(main())



