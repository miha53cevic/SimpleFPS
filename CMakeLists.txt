cmake_minimum_required(VERSION 3.10)

# Name of the project
project(SimpleFPS)

#set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

# Put executable inside of bin folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set source .cpp files
set(SOURCES 
    SourceCode/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Use C++17 standard 
set(CMAKE_CXX_STANDARD_REQUIRED 17)

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)