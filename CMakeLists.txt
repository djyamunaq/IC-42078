cmake_minimum_required(VERSION 3.16)
project(T3)

SET (CMAKE_BUILD_TYPE "Release")
#SET (CMAKE_BUILD_TYPE "Debug")

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17")
SET (CMAKE_CXX_FLAGS_RELEASE "-O3")
SET (CMAKE_CXX_FLAGS_DEBUG "-g3 -fsanitize=address")

if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()
if ( MSVC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
endif()

add_subdirectory(include)
add_subdirectory(src)

# Find packages
find_package(OpenCV REQUIRED)

# Create executable for exercise 1
add_executable (ex_1 ${T3_SOURCE_DIR}/src/ex_1.cpp)
include_directories(${T3_SOURCE_DIR}/include)
target_link_libraries(ex_1 ${OpenCV_LIBS})

add_executable (test_bit_stream ${T3_SOURCE_DIR}/src/test_bit_stream.cpp)
include_directories(${T3_SOURCE_DIR}/include)
target_link_libraries(test_bit_stream ${OpenCV_LIBS})

add_executable (test_golomb_coding ${T3_SOURCE_DIR}/src/test_golomb_coding.cpp)
include_directories(${T3_SOURCE_DIR}/include)