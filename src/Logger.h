#pragma once

#include <fstream>
#include <string>
#include <mutex>
#include <ctime>
#include <iomanip>
#include <sstream>

class Logger
{
private:
    static std::ofstream logFile;
    static std::mutex logMutex;
    static bool initialized;
    
    static std::string GetTimestamp()
    {
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    
public:
    static void Initialize(const std::string& filename = "cs2menu.log")
    {
        if (initialized) return;
        
        std::lock_guard<std::mutex> lock(logMutex);
        logFile.open(filename, std::ios::out | std::ios::app);
        initialized = true;
        
        if (logFile.is_open())
        {
            logFile << "\n========================================\n";
            logFile << "CS2MENU LOG - " << GetTimestamp() << "\n";
            logFile << "========================================\n";
            logFile.flush();
        }
    }
    
    static void Log(const std::string& level, const std::string& message)
    {
        if (!initialized) Initialize();
        
        std::lock_guard<std::mutex> lock(logMutex);
        if (logFile.is_open())
        {
            logFile << "[" << GetTimestamp() << "] [" << level << "] " << message << "\n";
            logFile.flush();
        }
    }
    
    static void Info(const std::string& message)
    {
        Log("INFO", message);
    }
    
    static void Warning(const std::string& message)
    {
        Log("WARN", message);
    }
    
    static void Error(const std::string& message)
    {
        Log("ERROR", message);
    }
    
    static void Debug(const std::string& message)
    {
#ifdef _DEBUG
        Log("DEBUG", message);
#endif
    }
    
    static void Shutdown()
    {
        std::lock_guard<std::mutex> lock(logMutex);
        if (logFile.is_open())
        {
            logFile << "========================================\n";
            logFile << "LOG CLOSED - " << GetTimestamp() << "\n";
            logFile << "========================================\n\n";
            logFile.close();
        }
        initialized = false;
    }
};

// Definiciones estáticas
inline std::ofstream Logger::logFile;
inline std::mutex Logger::logMutex;
inline bool Logger::initialized = false;

// Macros para facilitar el uso
#define LOG_INFO(msg) Logger::Info(msg)
#define LOG_WARN(msg) Logger::Warning(msg)
#define LOG_ERROR(msg) Logger::Error(msg)
#define LOG_DEBUG(msg) Logger::Debug(msg)
