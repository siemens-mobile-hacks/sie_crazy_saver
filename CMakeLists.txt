cmake_minimum_required(VERSION 3.31)

include(../sdk/config.cmake)
project(sieelf_crazy_saver C)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.c")

function(add_target target platform)
    add_executable(${target} ${SOURCES})
    target_sdk_setup(${target} ${platform})
    target_link_libraries(${target} PRIVATE crt crt_helper c gcc)
    target_compile_definitions(${target} PRIVATE SWILIB_MODERN)
    target_compile_options(${target} PRIVATE -Wall -Werror)
    set_target_properties(${target} PROPERTIES
            C_STANDARD 11
            C_STANDARD_REQUIRED ON
            C_EXTENSIONS OFF
            OUTPUT_NAME "SieCrazySaver_${platform}"
    )
endfunction()

add_target(SieCrazySaver_ELKA ELKA)
add_target(SieCrazySaver_NSG NSG)
add_target(SieCrazySaver_SG SG)
