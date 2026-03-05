#include "OffsetManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

OffsetManager& OffsetManager::Instance()
{
    static OffsetManager instance;
    return instance;
}

bool OffsetManager::Initialize()
{
    if (m_initialized)
        return true;
    
    std::cout << "[OffsetManager] Initializing..." << std::endl;
    
    // Try to load from JSON file first (cs2-dumper format)
    if (LoadFromJSON("offsets.json"))
    {
        std::cout << "[OffsetManager] Loaded offsets from offsets.json" << std::endl;
        m_lastLoadedFile = "offsets.json";
        m_initialized = true;
        return true;
    }
    
    // Try simple config format
    if (LoadFromConfig("offsets.cfg"))
    {
        std::cout << "[OffsetManager] Loaded offsets from offsets.cfg" << std::endl;
        m_lastLoadedFile = "offsets.cfg";
        m_initialized = true;
        return true;
    }
    
    // Fallback to hardcoded offsets
    std::cout << "[OffsetManager] No offset files found, using fallback offsets" << std::endl;
    LoadFallbackOffsets();
    m_initialized = true;
    
    return true;
}

bool OffsetManager::ReloadOffsets()
{
    if (!m_initialized)
    {
        std::cout << "[OffsetManager] Cannot reload - not initialized" << std::endl;
        return false;
    }
    
    std::cout << "[OffsetManager] Reloading offsets..." << std::endl;
    
    // Save old offsets for comparison
    auto oldOffsets = m_offsets;
    
    // Clear current offsets
    m_offsets.clear();
    
    // Try to reload from last loaded file
    bool success = false;
    if (!m_lastLoadedFile.empty())
    {
        if (m_lastLoadedFile.find(".json") != std::string::npos)
        {
            success = LoadFromJSON(m_lastLoadedFile);
        }
        else if (m_lastLoadedFile.find(".cfg") != std::string::npos)
        {
            success = LoadFromConfig(m_lastLoadedFile);
        }
    }
    
    if (!success)
    {
        // Restore old offsets if reload failed
        m_offsets = oldOffsets;
        std::cout << "[OffsetManager] Reload failed, keeping old offsets" << std::endl;
        return false;
    }
    
    // Compare and notify changes
    int changedCount = 0;
    for (const auto& pair : m_offsets)
    {
        auto it = oldOffsets.find(pair.first);
        if (it != oldOffsets.end() && it->second != pair.second)
        {
            NotifyOffsetChanged(pair.first, it->second, pair.second);
            changedCount++;
        }
    }
    
    std::cout << "[OffsetManager] Reload complete: " << changedCount << " offsets changed" << std::endl;
    return true;
}

bool OffsetManager::LoadFromJSON(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cout << "[OffsetManager] Failed to open: " << filePath << std::endl;
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();
    
    return ParseJSON(content);
}

bool OffsetManager::LoadFromConfig(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cout << "[OffsetManager] Failed to open: " << filePath << std::endl;
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();
    
    return ParseConfig(content);
}

uintptr_t OffsetManager::GetOffset(const std::string& name, uintptr_t fallback) const
{
    auto it = m_offsets.find(name);
    if (it != m_offsets.end())
        return it->second;
    
    if (fallback != 0)
    {
        std::cout << "[OffsetManager] Offset '" << name << "' not found, using fallback: 0x" 
                  << std::hex << fallback << std::dec << std::endl;
    }
    
    return fallback;
}

bool OffsetManager::HasOffset(const std::string& name) const
{
    return m_offsets.find(name) != m_offsets.end();
}

bool OffsetManager::ValidateOffsets() const
{
    if (m_offsets.empty())
    {
        std::cout << "[OffsetManager] Validation failed: No offsets loaded" << std::endl;
        return false;
    }
    
    // Check for critical offsets
    const char* criticalOffsets[] = {
        "dwEntityList",
        "dwLocalPlayerPawn",
        "dwViewMatrix"
    };
    
    bool allCriticalPresent = true;
    for (const char* name : criticalOffsets)
    {
        if (!HasOffset(name))
        {
            std::cout << "[OffsetManager] Critical offset missing: " << name << std::endl;
            allCriticalPresent = false;
        }
    }
    
    if (!allCriticalPresent)
    {
        std::cout << "[OffsetManager] Validation failed: Missing critical offsets" << std::endl;
        return false;
    }
    
    // Validate offset ranges (basic sanity check)
    for (const auto& pair : m_offsets)
    {
        if (!ValidateOffset(pair.first, pair.second))
        {
            std::cout << "[OffsetManager] Warning: Offset '" << pair.first 
                      << "' failed validation (0x" << std::hex << pair.second << std::dec << ")" << std::endl;
        }
    }
    
    std::cout << "[OffsetManager] Validation passed: " << m_offsets.size() << " offsets loaded" << std::endl;
    return true;
}

void OffsetManager::Clear()
{
    m_offsets.clear();
    m_gameVersion.clear();
    m_initialized = false;
}

void OffsetManager::SetOffset(const std::string& name, uintptr_t value)
{
    auto it = m_offsets.find(name);
    uintptr_t oldValue = (it != m_offsets.end()) ? it->second : 0;
    
    m_offsets[name] = value;
    
    // Notify if value changed
    if (oldValue != value)
    {
        NotifyOffsetChanged(name, oldValue, value);
    }
}

void OffsetManager::LoadFallbackOffsets()
{
    std::cout << "[OffsetManager] Loading fallback offsets (Build 14138)" << std::endl;
    
    // Main offsets (client.dll) - from SDK.h
    m_offsets["dwEntityList"] = 0x24AE628;
    m_offsets["dwLocalPlayerPawn"] = 0x2066DE0;
    m_offsets["dwViewMatrix"] = 0x2309420;
    m_offsets["dwLocalPlayerController"] = 0x22F1888;
    m_offsets["dwViewAngles"] = 0x2318668;
    m_offsets["dwGlobalVars"] = 0x2058FC0;
    m_offsets["dwGameRules"] = 0x230A160;
    m_offsets["dwGameEntitySystem_getHighestEntityIndex"] = 0x20A0;
    
    // Entity offsets
    m_offsets["m_iHealth"] = 0x76C;
    m_offsets["m_iMaxHealth"] = 0xB54;
    m_offsets["m_iTeamNum"] = 0xD70;
    m_offsets["m_pGameSceneNode"] = 0x338;
    m_offsets["m_vecAbsOrigin"] = 0xD0;
    m_offsets["m_hPlayerPawn"] = 0x90C;
    m_offsets["m_iIDEntIndex"] = 0x3EAC;
    m_offsets["m_bDormant"] = 0xE8;
    
    // Player offsets
    m_offsets["m_fFlags"] = 0x400;
    m_offsets["m_vecVelocity"] = 0x438;
    m_offsets["m_angEyeAngles"] = 0x3DD0;
    m_offsets["m_vecViewOffset"] = 0xD58;
    m_offsets["m_aimPunchAngle"] = 0x16CC;
    m_offsets["m_aimPunchCache"] = 0x1728;
    m_offsets["m_iShotsFired"] = 0x1414;
    m_offsets["m_bSpotted"] = 0x8;
    m_offsets["m_bIsScoped"] = 0x26F8;
    m_offsets["m_flFlashDuration"] = 0x1470;
    
    // Player controller offsets
    m_offsets["m_iszPlayerName"] = 0x6F8;
    
    // Weapon offsets
    m_offsets["m_hActiveWeapon"] = 0x12F8;
    
    // Weapon offsets
    m_offsets["m_pClippingWeapon"] = 0x1290;
    m_offsets["m_iClip1"] = 0x1564;
    m_offsets["m_iClip2"] = 0x1568;
    m_offsets["m_nFallbackPaintKit"] = 0x31B8;
    m_offsets["m_nFallbackSeed"] = 0x31BC;
    m_offsets["m_flFallbackWear"] = 0x31C0;
    
    m_gameVersion = "Build 14138 (2026-03-05)";
    
    std::cout << "[OffsetManager] Loaded " << m_offsets.size() << " fallback offsets" << std::endl;
}

bool OffsetManager::ParseJSON(const std::string& content)
{
    // Simple JSON parser for cs2-dumper format
    // Expected format: { "client.dll": { "dwEntityList": "0x123456", ... } }
    
    std::cout << "[OffsetManager] Parsing JSON..." << std::endl;
    
    size_t pos = 0;
    int offsetsFound = 0;
    
    // Look for "client.dll" section
    size_t clientPos = content.find("\"client.dll\"");
    if (clientPos == std::string::npos)
    {
        std::cout << "[OffsetManager] No client.dll section found in JSON" << std::endl;
        return false;
    }
    
    // Find the opening brace for client.dll section
    size_t braceStart = content.find('{', clientPos);
    if (braceStart == std::string::npos)
        return false;
    
    // Find the closing brace
    int braceCount = 1;
    size_t braceEnd = braceStart + 1;
    while (braceEnd < content.length() && braceCount > 0)
    {
        if (content[braceEnd] == '{') braceCount++;
        else if (content[braceEnd] == '}') braceCount--;
        braceEnd++;
    }
    
    // Extract client.dll section
    std::string clientSection = content.substr(braceStart, braceEnd - braceStart);
    
    // Parse key-value pairs
    pos = 0;
    while (pos < clientSection.length())
    {
        // Find key (between quotes)
        size_t keyStart = clientSection.find('\"', pos);
        if (keyStart == std::string::npos) break;
        keyStart++;
        
        size_t keyEnd = clientSection.find('\"', keyStart);
        if (keyEnd == std::string::npos) break;
        
        std::string key = clientSection.substr(keyStart, keyEnd - keyStart);
        
        // Find value (between quotes or colon)
        size_t colonPos = clientSection.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        size_t valueStart = clientSection.find_first_not_of(" \t\n\r", colonPos + 1);
        if (valueStart == std::string::npos) break;
        
        std::string value;
        if (clientSection[valueStart] == '\"')
        {
            // String value
            valueStart++;
            size_t valueEnd = clientSection.find('\"', valueStart);
            if (valueEnd == std::string::npos) break;
            value = clientSection.substr(valueStart, valueEnd - valueStart);
            pos = valueEnd + 1;
        }
        else
        {
            // Numeric value
            size_t valueEnd = clientSection.find_first_of(",}\n", valueStart);
            if (valueEnd == std::string::npos) valueEnd = clientSection.length();
            value = clientSection.substr(valueStart, valueEnd - valueStart);
            pos = valueEnd;
        }
        
        // Parse hex value
        uintptr_t offsetValue = ParseHexValue(value);
        if (offsetValue != 0 || value == "0" || value == "0x0")
        {
            m_offsets[key] = offsetValue;
            offsetsFound++;
        }
    }
    
    std::cout << "[OffsetManager] Parsed " << offsetsFound << " offsets from JSON" << std::endl;
    return offsetsFound > 0;
}

bool OffsetManager::ParseConfig(const std::string& content)
{
    // Parse simple config format: key=value
    std::cout << "[OffsetManager] Parsing config..." << std::endl;
    
    std::istringstream stream(content);
    std::string line;
    int offsetsFound = 0;
    
    while (std::getline(stream, line))
    {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == ';' || line[0] == '/')
            continue;
        
        // Find '=' separator
        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos)
            continue;
        
        // Extract key and value
        std::string key = line.substr(0, equalPos);
        std::string value = line.substr(equalPos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);
        
        if (key.empty() || value.empty())
            continue;
        
        // Parse hex value
        uintptr_t offsetValue = ParseHexValue(value);
        m_offsets[key] = offsetValue;
        offsetsFound++;
    }
    
    std::cout << "[OffsetManager] Parsed " << offsetsFound << " offsets from config" << std::endl;
    return offsetsFound > 0;
}

uintptr_t OffsetManager::ParseHexValue(const std::string& hexStr) const
{
    if (hexStr.empty())
        return 0;
    
    std::string str = hexStr;
    
    // Remove whitespace
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    
    // Handle "0x" prefix
    if (str.length() >= 2 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
        str = str.substr(2);
    
    // Convert hex to uintptr_t
    try
    {
        return std::stoull(str, nullptr, 16);
    }
    catch (...)
    {
        std::cout << "[OffsetManager] Failed to parse hex value: " << hexStr << std::endl;
        return 0;
    }
}

bool OffsetManager::ValidateOffset(const std::string& name, uintptr_t value) const
{
    // Basic sanity checks for offset values
    
    // Offsets starting with "dw" are usually larger (module-level offsets)
    if (name.length() >= 2 && name[0] == 'd' && name[1] == 'w')
    {
        // Module offsets should be reasonable (< 100MB)
        if (value > 0x6400000) // 100MB
            return false;
    }
    else if (name.length() >= 2 && name[0] == 'm' && name[1] == '_')
    {
        // Member offsets should be smaller (< 64KB for most cases)
        if (value > 0x10000) // 64KB
        {
            // Some member offsets can be larger, so just warn
            // Don't fail validation
        }
    }
    
    return true;
}

void OffsetManager::RegisterUpdateCallback(OffsetUpdateCallback callback)
{
    if (callback)
    {
        m_callbacks.push_back(callback);
        std::cout << "[OffsetManager] Registered update callback" << std::endl;
    }
}

void OffsetManager::UnregisterUpdateCallback(OffsetUpdateCallback callback)
{
    auto it = std::find(m_callbacks.begin(), m_callbacks.end(), callback);
    if (it != m_callbacks.end())
    {
        m_callbacks.erase(it);
        std::cout << "[OffsetManager] Unregistered update callback" << std::endl;
    }
}

void OffsetManager::NotifyOffsetChanged(const std::string& name, uintptr_t oldValue, uintptr_t newValue)
{
    std::cout << "[OffsetManager] Offset changed: " << name 
              << " (0x" << std::hex << oldValue << " -> 0x" << newValue << std::dec << ")" << std::endl;
    
    // Notify all registered callbacks
    for (auto callback : m_callbacks)
    {
        if (callback)
        {
            callback(name, oldValue, newValue);
        }
    }
}
