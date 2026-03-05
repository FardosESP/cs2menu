#pragma once

#include <Windows.h>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * OffsetManager - Dynamic offset loading and management system
 * 
 * Features:
 * - Parse offsets from cs2-dumper JSON/config files
 * - Offset validation and version checking
 * - Fallback mechanisms for missing offsets
 * - Runtime offset updates
 * - Notification system for offset changes
 */
class OffsetManager
{
public:
    // Callback type for offset update notifications
    using OffsetUpdateCallback = void(*)(const std::string& offsetName, uintptr_t oldValue, uintptr_t newValue);
    
    // Singleton access
    static OffsetManager& Instance();
    
    // Initialize and load offsets
    bool Initialize();
    
    // Load offsets from JSON file (cs2-dumper format)
    bool LoadFromJSON(const std::string& filePath);
    
    // Load offsets from simple config file (key=value format)
    bool LoadFromConfig(const std::string& filePath);
    
    // Reload offsets from file (triggers notifications for changed offsets)
    bool ReloadOffsets();
    
    // Get offset by name with fallback
    uintptr_t GetOffset(const std::string& name, uintptr_t fallback = 0) const;
    
    // Check if offset exists
    bool HasOffset(const std::string& name) const;
    
    // Validate loaded offsets (basic sanity checks)
    bool ValidateOffsets() const;
    
    // Get game version/build number
    std::string GetGameVersion() const { return m_gameVersion; }
    
    // Set game version
    void SetGameVersion(const std::string& version) { m_gameVersion = version; }
    
    // Get offset count
    size_t GetOffsetCount() const { return m_offsets.size(); }
    
    // Clear all offsets
    void Clear();
    
    // Manually set an offset (triggers notification if changed)
    void SetOffset(const std::string& name, uintptr_t value);
    
    // Load hardcoded fallback offsets
    void LoadFallbackOffsets();
    
    // Register callback for offset updates
    void RegisterUpdateCallback(OffsetUpdateCallback callback);
    
    // Unregister callback
    void UnregisterUpdateCallback(OffsetUpdateCallback callback);
    
private:
    OffsetManager() = default;
    ~OffsetManager() = default;
    
    // Prevent copying
    OffsetManager(const OffsetManager&) = delete;
    OffsetManager& operator=(const OffsetManager&) = delete;
    
    // Parse JSON content (simple parser for cs2-dumper format)
    bool ParseJSON(const std::string& content);
    
    // Parse config content (key=value format)
    bool ParseConfig(const std::string& content);
    
    // Convert hex string to uintptr_t
    uintptr_t ParseHexValue(const std::string& hexStr) const;
    
    // Validate individual offset
    bool ValidateOffset(const std::string& name, uintptr_t value) const;
    
    // Notify callbacks of offset change
    void NotifyOffsetChanged(const std::string& name, uintptr_t oldValue, uintptr_t newValue);
    
    // Storage
    std::unordered_map<std::string, uintptr_t> m_offsets;
    std::string m_gameVersion;
    std::string m_lastLoadedFile;
    bool m_initialized = false;
    
    // Callbacks
    std::vector<OffsetUpdateCallback> m_callbacks;
};
