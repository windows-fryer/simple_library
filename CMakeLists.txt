cmake_minimum_required(VERSION 3.21)
project(simple_library)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

IF (NOT CMAKE_BUILD_TYPE MATCHES Driver)
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
ELSE ()
    file(GLOB_RECURSE DRIVER
            "project_driver/*.h" ,
            "project_driver/*.hpp" ,
            "project_driver/*.c" ,
            "project_driver/*.cpp" ,
            "project_driver/**/*.h" ,
            "project_driver/**/*.hpp" ,
            "project_driver/**/*.c" ,
            "project_driver/**/*.cpp" ,
            )

    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

    set(KERNELHEADERS_INCLUDE_DIRS /usr/lib/modules/5.17.1-arch1-1/build/include)

    include_directories(${KERNELHEADERS_INCLUDE_DIRS})

    set(DRIVER_FILE simple_driver.ko)
    set(KBUILD_CMD $(MAKE) -C ${KERNELHEADERS_DIR} modules M=${CMAKE_CURRENT_BINARY_DIR} src=${CMAKE_CURRENT_SOURCE_DIR})

    # Generate the Kbuild file through cmake.
    FILE(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/Kbuild "obj-m := project_driver/linux/simple_driver.o")

    add_custom_command(OUTPUT ${DRIVER_FILE}
            COMMAND ${KBUILD_CMD}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            DEPENDS ${DRIVER} VERBATIM)

    add_library(simple_driver_static SHARED project_driver/linux/simple_driver.c)

    message(${KERNELHEADERS_INCLUDE_DIRS})

    include_directories("/usr/lib/modules/5.17.1-arch1-1/build/include")

    add_custom_target(simple_driver ALL DEPENDS ${DRIVER_FILE})
ENDIF ()