cmake_minimum_required(VERSION 3.5)
project(hashi VERSION 0.1 LANGUAGES CXX)

# set variables
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# test lib
find_package(Catch2 3 REQUIRED)

# create library "HashiLib"
file(GLOB_RECURSE
  HASHI_LIB_SOURCES CONFIGURE_DEPENDS "lib/hashi/include/*.hpp" "lib/hashi/src/*.cpp")
include_directories(${CMAKE_SOURCE_DIR}/lib/)
add_library(HashiLib ${HASHI_LIB_SOURCES})

# add tests
add_executable(hashi_test lib/hashi/test/hashi_test.cpp)
target_link_libraries(hashi_test PRIVATE Catch2::Catch2WithMain HashiLib)
add_custom_target(run_hashi_test
     COMMAND ./hashi_test
     DEPENDS hashi_test
     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
include(CTest)
include(Catch)
catch_discover_tests(hashi_test)

# add the executable
add_executable(hashi exe/main.cpp)
target_link_libraries(hashi HashiLib)
add_custom_target(run
     COMMAND ./hashi
     DEPENDS hashi
     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
