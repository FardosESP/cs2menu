# Memory Safety Unit Tests

## Overview

This file contains unit tests for the memory safety improvements made to the CS2 cheat codebase. The tests verify that null pointer checks, safe memory reading, and entity validation work correctly, specifically focusing on ESP rendering safety and invalid entity index handling.

## Test Coverage

The test suite covers:

### 1. Basic Memory Safety
   - SafeRead with null pointers
   - SafeRead with invalid pointers
   - IsValidPointer validation

### 2. Entity Validation
   - Entity IsValid() method with null pointers
   - Entity health range validation (0-500)
   - Entity team range validation (0-3)
   - PlayerPawn GetOriginSafe() with null pointers
   - GetOriginSafe with null scene nodes

### 3. Invalid Entity Index Handling
   - Negative entity indices
   - Out-of-bounds entity indices (> 8192)
   - Entity system bounds checking

### 4. ESP Rendering Null Pointer Safety (NEW)
   - ESP rendering with null EntitySystem pointer
   - ESP rendering with null ViewMatrix pointer
   - ESP rendering with invalid entity pointers
   - WorldToScreen with null ViewMatrix

## Running the Tests

### Option 1: Compile as Standalone Executable

To compile and run the tests as a standalone executable using MSVC:

```bash
# Using MSVC (requires Visual Studio Developer Command Prompt)
cl /EHsc /std:c++17 /I. src/MemorySafety.test.cpp src/Memory.cpp /Fe:MemorySafetyTests.exe
./MemorySafetyTests.exe
```

### Option 2: Using CMake (Recommended)

Add the following to CMakeLists.txt to build the test executable:

```cmake
# Memory Safety Tests
add_executable(MemorySafetyTests 
    src/MemorySafety.test.cpp 
    src/Memory.cpp
)
target_include_directories(MemorySafetyTests PRIVATE src)
if(WIN32)
    target_compile_options(MemorySafetyTests PRIVATE /std:c++17 /EHsc)
endif()
```

Then build and run:

```bash
cmake --build . --target MemorySafetyTests
./MemorySafetyTests.exe
```

### Option 3: Manual Verification

Since this is a game cheat that runs as a DLL, you can also verify the safety improvements by:

1. Injecting the DLL into CS2
2. Monitoring for crashes when ESP is enabled
3. Checking console output for validation messages
4. Testing with various game states (menu, in-game, spectating)

## Expected Results

All tests should pass with output:

```
=== Memory Safety Unit Tests ===

Running test: SafeRead_NullPointer... PASSED
Running test: SafeRead_InvalidPointer... PASSED
Running test: IsValidPointer_Null... PASSED
Running test: IsValidPointer_LowAddress... PASSED
Running test: Entity_IsValid_NullPointer... PASSED
Running test: Entity_IsValid_InvalidHealth... PASSED
Running test: Entity_HealthValidation... PASSED
Running test: Entity_TeamValidation... PASSED
Running test: PlayerPawn_GetOriginSafe_NullPointer... PASSED
Running test: GetOriginSafe_NullEntity... PASSED
Running test: GetOriginSafe_NullSceneNode... PASSED
Running test: EntitySystem_GetBaseEntity_InvalidIndex... PASSED
Running test: EntitySystem_IndexBoundsChecking... PASSED
Running test: ESP_NullEntitySystem... PASSED
Running test: ESP_NullViewMatrix... PASSED
Running test: ESP_InvalidEntityPointer... PASSED
Running test: WorldToScreen_NullViewMatrix... PASSED

=== All tests passed! ===
```

## Safety Improvements Tested

### 1. Safe Memory Reading (Memory.h)
- `SafeRead<T>()` - Exception-safe memory reading with __try/__except
- `IsValidPointer()` - Pointer validation before dereferencing

### 2. Entity Validation (SDK.h)
- `C_BaseEntity::IsValid()` - Comprehensive entity validation
  - Null pointer check
  - Health range validation (0-500)
  - Team range validation (0-3)
  - Exception handling
- `C_CSPlayerPawn::GetOriginSafe()` - Safe origin retrieval
  - Null pointer check
  - Null scene node check
  - Exception handling

### 3. Entity System (SDK.h)
- `C_CSGameEntitySystem::GetBaseEntity()` - Safe entity retrieval
  - Index bounds checking (0-8192)
  - Null pointer checks for list entries
  - Returns nullptr for invalid indices

### 4. ESP Rendering (Features.cpp)
- Comprehensive null pointer checks before accessing EntitySystem and ViewMatrix
- Pointer validation using IsValidPointer() before dereferencing
- Entity validation using IsValid() before accessing properties
- Exception handling around all entity iteration
- Safe local player data reading
- Early returns on invalid pointers

### 5. WorldToScreen (Features.cpp)
- ViewMatrix pointer validation
- Exception handling around matrix calculations
- Returns false on invalid input

### 6. Initialization (Features.cpp)
- Address validation before assigning global pointers
- Early return on invalid addresses
- Proper error logging

## Test Implementation Notes

- Tests use a simple assertion-based framework for minimal dependencies
- Some tests verify the logic exists in the implementation rather than executing it
- This is because the tests run standalone without the full game environment
- Real-world testing should be done by running the cheat in CS2
- The exception handling (`__try/__except`) is Windows-specific and provides robust crash protection

## Requirements Validated

These tests validate **Requirement 1.1** from the specification:
- Null pointer handling in ESP rendering
- Invalid entity index handling
- Memory safety in critical code paths
- Prevention of crashes due to invalid pointers
