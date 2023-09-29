cmake_minimum_required(VERSION 3.10)
project(AegrescitMedendo)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)

add_definitions(-w)

find_package (Threads)
find_package(Crow)
find_package(cpp-jwt REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

add_subdirectory(external/sqlite_orm)


AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/src/ SOURCES)

set(
    SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/storage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/engine.cpp
    ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

add_executable(AegrescitMedendo ${SRC})
target_link_libraries(AegrescitMedendo
    PUBLIC Crow::Crow
    sqlite_orm
    cpp-jwt::cpp-jwt
)

