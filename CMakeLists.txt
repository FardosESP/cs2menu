cmake_minimum_required(VERSION 3.15)
project(cs2menu CXX C) # Soporte para C++ y C

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Archivos fuente del proyecto
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "src/*.h")

# 2. Configurar ImGui (lib/imgui)
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui")
if(EXISTS "${IMGUI_DIR}/imgui.cpp")
    message(STATUS "ImGui detectado en ${IMGUI_DIR}")
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

# 3. Configurar MinHook (lib/minhook) - INCLUSIÓN MANUAL PARA EVITAR LNK2019
set(MINHOOK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/minhook")
if(EXISTS "${MINHOOK_DIR}/include/MinHook.h")
    message(STATUS "MinHook detectado en ${MINHOOK_DIR}")
    
    # Archivos fuente de MinHook
    list(APPEND MINHOOK_SOURCES 
        "${MINHOOK_DIR}/src/buffer.c"
        "${MINHOOK_DIR}/src/hook.c"
        "${MINHOOK_DIR}/src/trampoline.c"
        "${MINHOOK_DIR}/src/hde/hde32.c"
        "${MINHOOK_DIR}/src/hde/hde64.c"
    )
    list(APPEND SOURCE_FILES ${MINHOOK_SOURCES})
    include_directories("${MINHOOK_DIR}/include")
    include_directories("${MINHOOK_DIR}/src")
    include_directories("${MINHOOK_DIR}/src/hde")
    
    # Desactivar Precompiled Headers para archivos .c en MSVC
    if(MSVC)
        set_source_files_properties(${MINHOOK_SOURCES} PROPERTIES COMPILE_FLAGS "/Y-")
    endif()
endif()

# 4. Crear la DLL (Excluyendo el Launcher)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/Launcher.cpp")
    list(REMOVE_ITEM SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/Launcher.cpp")
endif()
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES})

# 5. Crear el Launcher (Ejecutable independiente)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/Launcher.cpp")
    add_executable(cs2menu_launcher src/Launcher.cpp)
    target_include_directories(cs2menu_launcher PRIVATE src)
    if(WIN32)
        target_link_libraries(cs2menu_launcher PRIVATE user32.lib advapi32.lib)
    endif()
    # Esta línea asegura que al compilar la DLL también se compile el Launcher
    add_dependencies(${PROJECT_NAME} cs2menu_launcher)
endif()

# 6. Configuración final para Windows
target_include_directories(${PROJECT_NAME} PRIVATE src)

if(WIN32)
    add_compile_options(/W4 /wd4201 /wd4100 /wd4189 /wd4127 /wd4324 /wd4458 /wd4505 /wd4826 /wd4668 /wd4706 /wd4714 /wd4710 /wd4738 /wd4746 /wd4774 /wd4800 /wd4820 /wd4996)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    target_link_libraries(${PROJECT_NAME} PRIVATE user32.lib gdi32.lib d3d11.lib dxgi.lib)
endif()

# 7. Salida unificada en la carpeta Release
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release"
)

if(TARGET cs2menu_launcher)
    set_target_properties(cs2menu_launcher PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release"
    )
endif()
