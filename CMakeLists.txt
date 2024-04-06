cmake_minimum_required(VERSION 3.28.2)

include(../sdk/config.cmake)
project(sieelf_crazy_saver C)

include_directories("../libsie/src/include")
link_libraries(crt crt_helper c gcc ft sie)
file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(sie_crazy_saver_nsg ${SOURCES})
target_sdk_setup(sie_crazy_saver_nsg NSG)
target_link_directories(sie_crazy_saver_nsg PUBLIC "../libsie/build/lib/NSG")
target_compile_options(sie_crazy_saver_nsg BEFORE PUBLIC -std=gnu99 -Wall -Werror)

add_executable(sie_crazy_saver_elka ${SOURCES})
target_sdk_setup(sie_crazy_saver_elka ELKA)
target_link_directories(sie_crazy_saver_elka PUBLIC "../libsie/build/lib/ELKA")
target_compile_options(sie_crazy_saver_elka BEFORE PUBLIC -std=gnu99 -Wall -Werror)
