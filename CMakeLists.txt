cmake_minimum_required(VERSION 3.15)
project(TaxonomyBackend CXX)

# Use modern C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable highest compiler optimizations for release builds
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Find Drogon and its dependencies
find_package(Drogon CONFIG REQUIRED)

# Collect all source files
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# Define the executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Include the src directory so files can find each other cleanly
target_include_directories(${PROJECT_NAME} PRIVATE src)

# Link Drogon to our executable
target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon)