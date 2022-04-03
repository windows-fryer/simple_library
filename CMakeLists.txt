cmake_minimum_required(VERSION 3.21)
project(simple_library)

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

file(GLOB_RECURSE TEST
        "project_test/*.h" ,
        "project_test/*.hpp" ,
        "project_test/*.c" ,
        "project_test/*.cpp" ,
        "project_test/**/*.h" ,
        "project_test/**/*.hpp" ,
        "project_test/**/*.c" ,
        "project_test/**/*.cpp" ,
        )


add_executable(${PROJECT_NAME} ${SRC})
add_executable(${PROJECT_NAME}_test ${TEST})