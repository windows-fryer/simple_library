cmake_minimum_required(VERSION 3.21)
project(library_testing)

set(CMAKE_CXX_STANDARD 23)

# we probably dont need this many, only 1 should be fine. this looks funni
file(GLOB_RECURSE SRC
    "project/*.h" ,
    "project/*.hpp" ,
    "project/*.c" ,
    "project/*.cpp" ,
    "project/**/*.h" ,
    "project/**/*.hpp" ,
    "project/**/*.c" ,
    "project/**/*.cpp" ,
)


add_executable(${PROJECT_NAME} ${SRC})