@echo off
echo Building AI Trading Bot...

REM Create build directory if it doesn't exist
if not exist "build" mkdir build
cd build

REM Configure with CMake
echo Configuring project...
cmake .. -G "Visual Studio 17 2022" -A x64
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

REM Build the project
echo Building project...
cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo Build completed successfully!
echo.
echo To run the trading bot:
echo   cd bin\Release
echo   trading_bot.exe ..\..\data\sample_data.csv SMA_CROSSOVER
echo.
pause
