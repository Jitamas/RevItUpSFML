cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

## nlohmann/json for easy JSON parsing (fetch before using the target)
FetchContent_Declare(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

## SFML
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

## Find CURL (system package or provided by vcpkg/toolchain)
find_package(CURL REQUIRED)

add_executable(main src/main.cpp)
target_compile_features(main PRIVATE cxx_std_17)
target_link_libraries(main PRIVATE SFML::Graphics CURL::libcurl nlohmann_json::nlohmann_json)

add_executable(search_tool src/search.cpp)
target_compile_features(search_tool PRIVATE cxx_std_17)
target_link_libraries(search_tool PRIVATE CURL::libcurl nlohmann_json::nlohmann_json)
