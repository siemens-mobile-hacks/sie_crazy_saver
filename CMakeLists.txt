cmake_minimum_required(VERSION 3.28.2)

include(../sdk/config.cmake)
project(sieelf_crazy_saver C)

link_libraries(crt crt_helper c gcc)
file(GLOB_RECURSE SOURCES "src/*.c")

add_definitions(-DSWILIB_MODERN)

add_executable(SieCrazySaver_ELKA ${SOURCES})
target_sdk_setup(SieCrazySaver_ELKA ELKA)
target_compile_options(SieCrazySaver_ELKA BEFORE PUBLIC -std=c11 -Wall -Werror)

add_executable(SieCrazySaver_NSG ${SOURCES})
target_sdk_setup(SieCrazySaver_NSG NSG)
target_compile_options(SieCrazySaver_NSG BEFORE PUBLIC -std=c11 -Wall -Werror)

add_executable(SieCrazySaver_SG ${SOURCES})
target_sdk_setup(SieCrazySaver_SG SG)
target_compile_options(SieCrazySaver_SG BEFORE PUBLIC -std=c11 -Wall -Werror)
