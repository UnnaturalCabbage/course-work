project(cmake_vcpkg_template)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(default_build_type "Release")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  set(default_build_type "Debug")
endif()

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})

find_package(jsoncpp REQUIRED)

file(GLOB CPPS "src/*.cpp")
add_definitions(-Wall -O2)
add_executable(${PROJECT_NAME} ${CPPS})
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(${PROJECT_NAME} jsoncpp_lib)
file(COPY "./src/data.json" DESTINATION "${CMAKE_BINARY_DIR}/Release") #${CMAKE_BUILD_TYPE}
