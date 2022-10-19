cmake_minimum_required(VERSION 3.13)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
  cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/.deps)
FetchContent_Declare(
	catch2
	URL https://github.moeyy.xyz/https://github.com/catchorg/Catch2/archive/refs/tags/v3.1.1.zip
)
FetchContent_MakeAvailable(catch2)

project(catch2_demo)
add_executable(main main.cpp)
target_link_libraries(main PRIVATE Catch2 Catch2WithMain)
