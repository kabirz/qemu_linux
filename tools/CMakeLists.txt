cmake_minimum_required(VERSION 3.13)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
  cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)
FetchContent_Declare(
	cli
	URL https://github.com/daniele77/cli/archive/refs/tags/v2.0.2.zip
)
FetchContent_MakeAvailable(cli)

project(periphery_tool)
aux_source_directory(src TOOL_SRC)
add_executable(periphery_tool src/main.cpp ${TOOL_SRC})

target_link_libraries(periphery_tool PRIVATE cli)
target_include_directories(periphery_tool PRIVATE include)
