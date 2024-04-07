cmake_minimum_required(VERSION 3.28.2)

include(../sdk/config.cmake)
project(sieelf_crazy_saver C)

include_directories("../libsie/src/include")
link_libraries(crt crt_helper c gcc ft sie)
file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(SieCrazySaver_NSG ${SOURCES})
target_sdk_setup(SieCrazySaver_NSG NSG)
target_link_directories(SieCrazySaver_NSG PUBLIC "../libsie/build/lib/NSG")
target_compile_options(SieCrazySaver_NSG BEFORE PUBLIC -std=gnu99 -Wall -Werror)

add_executable(SieCrazySaver_ELKA ${SOURCES})
target_sdk_setup(SieCrazySaver_ELKA ELKA)
target_link_directories(SieCrazySaver_ELKA PUBLIC "../libsie/build/lib/ELKA")
target_compile_options(SieCrazySaver_ELKA BEFORE PUBLIC -std=gnu99 -Wall -Werror)
