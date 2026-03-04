cmake_minimum_required(VERSION 3.15)

project(cs2_internal_edu_project CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configuración para Windows
if(WIN32)
    # Deshabilitar algunas advertencias de seguridad de MSVC
    add_compile_options(/W4 /wd4201 /wd4100 /wd4189 /wd4127 /wd4324 /wd4458 /wd4505 /wd4826 /wd4668 /wd4706 /wd4714 /wd4710 /wd4738 /wd4746 /wd4774 /wd4800 /wd4820 /wd4996)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    # Enlazar librerías de Windows
    list(APPEND LIBS_WIN32 user32.lib gdi32.lib shell32.lib ole32.lib advapi32.lib d3d11.lib dxgi.lib)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS_WIN32})
endif()

# Directorios de inclusión
include_directories(src)

# Añadir ImGui como submódulo y configurarlo
add_subdirectory(lib/imgui)

# Añadir MinHook como submódulo y configurarlo
add_subdirectory(lib/minhook)

# Definir los archivos fuente de la DLL
set(SOURCE_FILES
    src/DllMain.cpp
    src/Hooks.cpp
    src/ImGui_Renderer.cpp
    src/main.cpp
)

# Crear la librería compartida (DLL)
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

# Enlazar ImGui y MinHook
target_link_libraries(${PROJECT_NAME} PRIVATE imgui minhook)

# Configurar propiedades de la DLL
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release"
)

# Descargar submódulos si no están presentes (para git clone --recursive)
# Esto es más para un flujo de trabajo de git, pero es bueno tenerlo.
execute_process(COMMAND git submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib
                RESULT_VARIABLE GIT_SUBMODULE_RESULT)
if(NOT "${GIT_SUBMODULE_RESULT}" STREQUAL "0")
    message(WARNING "Failed to update git submodules. Please ensure ImGui and MinHook are in lib/imgui and lib/minhook respectively.")
endif()
