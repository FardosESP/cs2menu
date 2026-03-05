#include "OffsetManager.h"
#include <iostream>
#include <cassert>
#include <fstream>

// Simple test framework
#define TEST(name) void name()
#define ASSERT_TRUE(condition) assert(condition)
#define ASSERT_FALSE(condition) assert(!(condition))
#define ASSERT_EQ(a, b) assert((a) == (b))
#define RUN_TEST(test) do { std::cout << "Running " #test "..." << std::endl; test(); std::cout << "  PASSED" << std::endl; } while(0)

TEST(TestSingletonAccess)
{
    OffsetManager& mgr1 = OffsetManager::Instance();
    OffsetManager& mgr2 = OffsetManager::Instance();
    
    // Should be the same instance
    ASSERT_EQ(&mgr1, &mgr2);
}

TEST(TestFallbackOffsets)
{
    OffsetManager& mgr = OffsetManager::Instance();
    mgr.Clear();
    
    mgr.LoadFallbackOffsets();
    
    // Check critical offsets are loaded
    ASSERT_TRUE(mgr.HasOffset("dwEntityList"));
    ASSERT_TRUE(mgr.HasOffset("dwLocalPlayerPawn"));
    ASSERT_TRUE(mgr.HasOffset("dwViewMatrix"));
    ASSERT_TRUE(mgr.HasOffset("m_iHealth"));
    ASSERT_TRUE(mgr.HasOffset("m_iTeamNum"));
    
    // Check specific values
    ASSERT_EQ(mgr.GetOffset("dwEntityList"), 0x24AE628);
    ASSERT_EQ(mgr.GetOffset("m_iHealth"), 0x76C);
    
    // Check offset count
    ASSERT_TRUE(mgr.GetOffsetCount() > 20);
}

TEST(TestGetOffsetWithFallback)
{
    OffsetManager& mgr = OffsetManager::Instance();
    mgr.Clear();
    
    mgr.SetOffset("testOffset", 0x1234);
    
    // Existing offset
    ASSERT_EQ(mgr.GetOffset("testOffset"), 0x1234);
    
    // Non-existing offset with fallback
    ASSERT_EQ(mgr.GetOffset("nonExistent", 0x9999), 0x9999);
    
    // Non-existing offset without fallback
    ASSERT_EQ(mgr.GetOffset("nonExistent"), 0);
}

TEST(TestSetOffset)
{
    OffsetManager& mgr = OffsetManager::Instance();
    mgr.Clear();
    
    ASSERT_FALSE(mgr.HasOffset("customOffset"));
    
    mgr.SetOffset("customOffset", 0xABCD);
    
    ASSERT_TRUE(mgr.HasOffset("customOffset"));
    ASSERT_EQ(mgr.GetOffset("customOffset"), 0xABCD);
}

TEST(TestValidateOffsets)
{
    OffsetManager& mgr = OffsetManager::Instance();
    mgr.Clear();
    
    // Empty offsets should fail validation
    ASSERT_FALSE(mgr.ValidateOffsets());
    
    // Add critical offsets
    mgr.SetOffset("dwEntityList", 0x1000000);
    mgr.SetOffset("dwLocalPlayerPawn", 0x2000000);
    mgr.SetOffset("dwViewMatrix", 0x3000000);
    
    // Should pass validation now
    ASSERT_TRUE(mgr.ValidateOffsets());
}

TEST(TestParseConfigFormat)
{
    OffsetManager& mgr = OffsetManager::Instance();
    mgr.Clear();
    
    // Create a test config file
    std::ofstream file("test_offsets.cfg");
    file << "# Test config file\n";
    file << "dwEntityList=0x24AE628\n";
    file << "dwLocalPlayerPawn=0x2066DE0\n";
    file << "m_iHealth=0x76C\n";
    file << "\n";
    file << "# Comment line\n";
    file << "m_iTeamNum=0xD70\n";
    file.close();
    
    // Load from config
    ASSERT_TRUE(mgr.LoadFromConfig("test_offsets.cfg"));
    
    // Verify loaded offsets
    ASSERT_TRUE(mgr.HasOffset("dwEntityList"));
    ASSERT_TRUE(mgr.HasOffset("dwLocalPlayerPawn"));
    ASSERT_TRUE(mgr.HasOffset("m_iHealth"));
    ASSERT_TRUE(mgr.HasOffset("m_iTeamNum"));
    
    ASSERT_EQ(mgr.GetOffset("dwEntityList"), 0x24AE628);
    ASSERT_EQ(mgr.GetOffset("m_iHealth"), 0x76C);
    
    // Cleanup
    std::remove("test_offsets.cfg");
}

TEST(TestParseJSONFormat)
{
    OffsetManager& mgr = OffsetManager::Instance();
    mgr.Clear();
    
    // Create a test JSON file (simplified cs2-dumper format)
    std::ofstream file("test_offsets.json");
    file << "{\n";
    file << "  \"client.dll\": {\n";
    file << "    \"dwEntityList\": \"0x24AE628\",\n";
    file << "    \"dwLocalPlayerPawn\": \"0x2066DE0\",\n";
    file << "    \"dwViewMatrix\": \"0x2309420\",\n";
    file << "    \"m_iHealth\": \"0x76C\",\n";
    file << "    \"m_iTeamNum\": \"0xD70\"\n";
    file << "  }\n";
    file << "}\n";
    file.close();
    
    // Load from JSON
    ASSERT_TRUE(mgr.LoadFromJSON("test_offsets.json"));
    
    // Verify loaded offsets
    ASSERT_TRUE(mgr.HasOffset("dwEntityList"));
    ASSERT_TRUE(mgr.HasOffset("dwLocalPlayerPawn"));
    ASSERT_TRUE(mgr.HasOffset("dwViewMatrix"));
    ASSERT_TRUE(mgr.HasOffset("m_iHealth"));
    ASSERT_TRUE(mgr.HasOffset("m_iTeamNum"));
    
    ASSERT_EQ(mgr.GetOffset("dwEntityList"), 0x24AE628);
    ASSERT_EQ(mgr.GetOffset("dwViewMatrix"), 0x2309420);
    
    // Cleanup
    std::remove("test_offsets.json");
}

TEST(TestInitializeWithFallback)
{
    OffsetManager& mgr = OffsetManager::Instance();
    mgr.Clear();
    
    // Initialize should succeed even without files (uses fallback)
    ASSERT_TRUE(mgr.Initialize());
    
    // Should have loaded fallback offsets
    ASSERT_TRUE(mgr.HasOffset("dwEntityList"));
    ASSERT_TRUE(mgr.GetOffsetCount() > 0);
}

TEST(TestClear)
{
    OffsetManager& mgr = OffsetManager::Instance();
    
    mgr.SetOffset("test1", 0x100);
    mgr.SetOffset("test2", 0x200);
    mgr.SetGameVersion("Test Version");
    
    ASSERT_TRUE(mgr.GetOffsetCount() > 0);
    ASSERT_FALSE(mgr.GetGameVersion().empty());
    
    mgr.Clear();
    
    ASSERT_EQ(mgr.GetOffsetCount(), 0);
    ASSERT_TRUE(mgr.GetGameVersion().empty());
}

int main()
{
    std::cout << "=== OffsetManager Unit Tests ===" << std::endl << std::endl;
    
    try
    {
        RUN_TEST(TestSingletonAccess);
        RUN_TEST(TestFallbackOffsets);
        RUN_TEST(TestGetOffsetWithFallback);
        RUN_TEST(TestSetOffset);
        RUN_TEST(TestValidateOffsets);
        RUN_TEST(TestParseConfigFormat);
        RUN_TEST(TestParseJSONFormat);
        RUN_TEST(TestInitializeWithFallback);
        RUN_TEST(TestClear);
        
        std::cout << std::endl << "=== All tests passed! ===" << std::endl;
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cout << std::endl << "=== Test failed with exception: " << e.what() << " ===" << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cout << std::endl << "=== Test failed with unknown exception ===" << std::endl;
        return 1;
    }
}
