# Project definition
cmake_minimum_required(VERSION 4.0)
project(voxel-meshing)

# Source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(INCL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(RES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res")
set(SOURCES "${SRC_DIR}/main.cpp" "${SRC_DIR}/renderer.cpp" "${SRC_DIR}/renderer.h" 
"${INCL_DIR}/learnopengl/camera.cpp" "${INCL_DIR}/learnopengl/camera.h"
"${SRC_DIR}/chunk.cpp" "${SRC_DIR}/chunk.h" "${SRC_DIR}/vox.h" "${INCL_DIR}/ogt_vox.h"
"${INCL_DIR}/imgui/imconfig.h" "${INCL_DIR}/imgui/imgui.h" "${INCL_DIR}/imgui/imgui_impl_glfw.h"
"${INCL_DIR}/imgui/imgui_impl_opengl3.h" "${INCL_DIR}/imgui/imgui_impl_opengl3_loader.h"
"${INCL_DIR}/imgui/imgui_internal.h" "${INCL_DIR}/imgui/imstb_rectpack.h"
"${INCL_DIR}/imgui/imstb_textedit.h" "${INCL_DIR}/imgui/imstb_truetype.h"
"${INCL_DIR}/imgui/imgui.cpp" "${INCL_DIR}/imgui/imgui_demo.cpp" "${INCL_DIR}/imgui/imgui_draw.cpp"
"${INCL_DIR}/imgui/imgui_impl_glfw.cpp" "${INCL_DIR}/imgui/imgui_impl_opengl3.cpp"
"${INCL_DIR}/imgui/imgui_tables.cpp" "${INCL_DIR}/imgui/imgui_widgets.cpp")

# Executable definition and properties
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

# GLFW
set(GLFW_DIR "${LIB_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory("${GLFW_DIR}")
target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# glad
set(GLAD_DIR "${LIB_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")

# include folder
target_include_directories(${PROJECT_NAME} PRIVATE "${INCL_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE "${RES_DIR}")
