cmake_minimum_required(VERSION 3.15)
project(cs2menu CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Buscar archivos fuente en src/
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "src/*.h")

# 2. Añadir ImGui (si existe en lib/imgui)
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

# 3. Añadir MinHook (si existe en lib/minhook)
set(MINHOOK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/minhook")
if(EXISTS "${MINHOOK_DIR}/include/MinHook.h")
    message(STATUS "MinHook detectado en ${MINHOOK_DIR}")
    file(GLOB MINHOOK_SOURCES 
        "${MINHOOK_DIR}/src/*.c" 
        "${MINHOOK_DIR}/src/hde/*.c"
    )
    list(APPEND SOURCE_FILES ${MINHOOK_SOURCES})
    include_directories("${MINHOOK_DIR}/include")
endif()

# 4. Crear la DLL
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES})

# 5. Configurar inclusiones y librerías
target_include_directories(${PROJECT_NAME} PRIVATE src)

if(WIN32)
    add_compile_options(/W4 /wd4201 /wd4100 /wd4189 /wd4127 /wd4324 /wd4458 /wd4505 /wd4826 /wd4668 /wd4706 /wd4714 /wd4710 /wd4738 /wd4746 /wd4774 /wd4800 /wd4820 /wd4996)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    target_link_libraries(${PROJECT_NAME} PRIVATE user32.lib gdi32.lib d3d11.lib dxgi.lib)
endif()

# 6. Salida de la DLL
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release"
)
