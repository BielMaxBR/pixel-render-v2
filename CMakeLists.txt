cmake_minimum_required(VERSION 3.15)
project(PixelRender)

set(CMAKE_CXX_STANDARD 17)

# --- CONFIGURAÇÕES QUE ESTAVAM NO .BAT ---
# Ativa a geração do compile_commands.json automaticamente
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "" FORCE)

# Define a flag de pré-processador para o seu código/Raylib
add_compile_definitions(GRAPHICS_API_OPENGL_11)
# -----------------------------------------

# Diretórios
set(SRC_DIR src)
set(EXTERNAL_DIR external)
set(IMGUI_DIR ${EXTERNAL_DIR}/imgui)
set(RLIMGUI_DIR ${EXTERNAL_DIR}/rlimgui)
set(IMGUIFILEDIALOG_DIR ${EXTERNAL_DIR}/imguifiledialog)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

# Arquivos do seu jogo
file(GLOB GAME_SOURCES ${SRC_DIR}/*.cpp)
file(GLOB IMGUI_SRC ${IMGUI_DIR}/*.cpp)
file(GLOB RLIMGUI_SRC ${RLIMGUI_DIR}/*.cpp)
file(GLOB IMGUIFILEDIALOG_SRC ${IMGUIFILEDIALOG_DIR}/*.cpp)

# ImGui
add_library(imgui STATIC ${IMGUI_SRC})
target_include_directories(imgui PUBLIC ${IMGUI_DIR}/include)

# rlImGui
add_library(rlimgui STATIC ${RLIMGUI_SRC})
target_include_directories(rlimgui PUBLIC ${IMGUI_DIR}/include ${RLIMGUI_DIR}/include include)

# ImGuiFileDialog
add_library(imguifiledialog STATIC ${IMGUIFILEDIALOG_SRC})
target_include_directories(imguifiledialog PUBLIC ${IMGUI_DIR}/include ${IMGUIFILEDIALOG_DIR}/include)

# Binário final do jogo
add_executable(PixelRender ${GAME_SOURCES})
set_target_properties(PixelRender PROPERTIES OUTPUT_NAME "Pixel Render")

target_include_directories(PixelRender PRIVATE
    include
    ${IMGUI_DIR}/include
    ${RLIMGUI_DIR}/include
    ${IMGUIFILEDIALOG_DIR}/include
)

target_link_libraries(PixelRender PRIVATE
  raylib
  imgui
  imguifiledialog
  rlimgui
)

if(WIN32)
    target_link_directories(PixelRender PRIVATE lib/windows)
    target_link_libraries(PixelRender PRIVATE gdi32 winmm imm32 shell32)
elseif(UNIX AND NOT APPLE)
    target_link_directories(PixelRender PRIVATE lib/linux)
    target_link_libraries(PixelRender PRIVATE GL m pthread dl rt X11)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O2")

# --- ALVO MULTIPLATAFORMA PARA COMPILAR E RODAR ---
# $<TARGET_FILE:PixelRender> resolve para build/out/Pixel Render.exe no Windows
# e para build/out/Pixel Render no Linux, tratando as barras automaticamente.
add_custom_target(run
    COMMAND $<TARGET_FILE:PixelRender>
    DEPENDS PixelRender
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
    COMMENT "Executando Pixel Render..."
)