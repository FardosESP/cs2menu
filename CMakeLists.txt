cmake_minimum_required(VERSION 3.15)
project(cs2menu CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Archivos fuente de la DLL (excluir Launcher y tests)
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "src/*.h")
list(REMOVE_ITEM SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/Launcher.cpp")
# Excluir archivos de test
file(GLOB_RECURSE TEST_FILES "src/*.test.cpp")
list(REMOVE_ITEM SOURCE_FILES ${TEST_FILES})

# 2. ImGui
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui")
if(EXISTS "${IMGUI_DIR}/imgui.cpp")
    list(APPEND SOURCE_FILES
            "${IMGUI_DIR}/imgui.cpp"
            "${IMGUI_DIR}/imgui_draw.cpp"
            "${IMGUI_DIR}/imgui_widgets.cpp"
            "${IMGUI_DIR}/imgui_tables.cpp"
            "${IMGUI_DIR}/imgui_demo.cpp"
            "${IMGUI_DIR}/backends/imgui_impl_dx11.cpp"
            "${IMGUI_DIR}/backends/imgui_impl_win32.cpp"
    )
    include_directories("${IMGUI_DIR}" "${IMGUI_DIR}/backends")
endif()

# 3. MinHook
set(MINHOOK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/minhook")
if(EXISTS "${MINHOOK_DIR}/include/MinHook.h")
    set(MINHOOK_SOURCES
            "${MINHOOK_DIR}/src/buffer.c"
            "${MINHOOK_DIR}/src/hook.c"
            "${MINHOOK_DIR}/src/trampoline.c"
            "${MINHOOK_DIR}/src/hde/hde32.c"
            "${MINHOOK_DIR}/src/hde/hde64.c"
    )
    list(APPEND SOURCE_FILES ${MINHOOK_SOURCES})
    include_directories("${MINHOOK_DIR}/include" "${MINHOOK_DIR}/src" "${MINHOOK_DIR}/src/hde")
    if(MSVC)
        set_source_files_properties(${MINHOOK_SOURCES} PROPERTIES COMPILE_FLAGS "/Y-")
    endif()
endif()

# 4. DLL principal
add_library(cs2menu SHARED ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(cs2menu PRIVATE src)

if(WIN32)
    target_compile_options(cs2menu PRIVATE /W4 /MD
            /wd4201 /wd4100 /wd4189 /wd4127 /wd4324
            /wd4458 /wd4505 /wd4826 /wd4668 /wd4706
            /wd4714 /wd4710 /wd4738 /wd4746 /wd4774
            /wd4800 /wd4820 /wd4996
    )
    target_compile_definitions(cs2menu PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_link_libraries(cs2menu PRIVATE
            user32.lib gdi32.lib d3d11.lib dxgi.lib psapi.lib dwmapi.lib
    )
endif()

# 5. Launcher
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/Launcher.cpp")
    add_executable(cs2menu_launcher src/Launcher.cpp)
    target_include_directories(cs2menu_launcher PRIVATE src)
    if(WIN32)
        target_compile_options(cs2menu_launcher PRIVATE /MD)
        target_link_libraries(cs2menu_launcher PRIVATE user32.lib advapi32.lib)
    endif()
    add_dependencies(cs2menu_launcher cs2menu)
endif()

# 6. Target "ALL" para compilar todo junto con un solo Build
# Esto hace que al pulsar Build en CLion se compilen DLL + Launcher a la vez
add_custom_target(build_all ALL
        DEPENDS cs2menu $<TARGET_NAME_IF_EXISTS:cs2menu_launcher>
)

# 7. Directorio de salida unificado
set_target_properties(cs2menu PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release"
)
if(TARGET cs2menu_launcher)
    set_target_properties(cs2menu_launcher PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release"
    )
endif()

# 8. Tests (opcional)
# Test para OffsetManager
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/OffsetManager.test.cpp")
    add_executable(test_offsetmanager
            src/OffsetManager.test.cpp
            src/OffsetManager.cpp
    )
    target_include_directories(test_offsetmanager PRIVATE src)
    if(WIN32)
        target_compile_options(test_offsetmanager PRIVATE /MD)
    endif()
    set_target_properties(test_offsetmanager PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release"
    )
endif()

# Test para MemorySafety
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/MemorySafety.test.cpp")
    add_executable(test_memorysafety
            src/MemorySafety.test.cpp
            src/Memory.cpp
    )
    target_include_directories(test_memorysafety PRIVATE src)
    if(WIN32)
        target_compile_options(test_memorysafety PRIVATE /MD)
        target_link_libraries(test_memorysafety PRIVATE psapi.lib)
    endif()
    set_target_properties(test_memorysafety PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release"
    )
endif()


# Test for Offset Update Notifications
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/OffsetUpdateNotification.test.cpp")
    add_executable(test_offset_notifications
            src/OffsetUpdateNotification.test.cpp
            src/OffsetManager.cpp
    )
    target_include_directories(test_offset_notifications PRIVATE src)
    if(WIN32)
        target_compile_options(test_offset_notifications PRIVATE /MD)
    endif()
    set_target_properties(test_offset_notifications PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release"
    )
endif()
