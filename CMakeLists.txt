cmake_minimum_required(VERSION 3.14)
project(vortex-data-engine VERSION 1.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wno-sign-conversion)

option(VDE_SANITIZE "Enable AddressSanitizer and UBSan" OFF)
if(VDE_SANITIZE)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -g)
    add_link_options(-fsanitize=address,undefined)
endif()

option(VDE_FUZZ "Build fuzzing targets" OFF)
if(VDE_FUZZ)
    add_compile_definitions(FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION)
endif()

file(GLOB_RECURSE LIB_SOURCES "src/**/*.cc")

add_library(vde STATIC ${LIB_SOURCES})
target_include_directories(vde PUBLIC include)

add_executable(vdx_inspect tools/vdx_inspect.cc)
target_link_libraries(vdx_inspect vde)

add_executable(vdx_validate tools/vdx_validate.cc)
target_link_libraries(vdx_validate vde)

add_executable(vdx_convert tools/vdx_convert.cc)
target_link_libraries(vdx_convert vde)

add_executable(vdx_query_cli tools/vdx_query_cli.cc)
target_link_libraries(vdx_query_cli vde)

add_executable(vdx_benchmark tools/vdx_benchmark.cc)
target_link_libraries(vdx_benchmark vde)

add_executable(vdx_dump tools/vdx_dump.cc)
target_link_libraries(vdx_dump vde)

add_executable(vdx_pack tools/vdx_pack.cc)
target_link_libraries(vdx_pack vde)

enable_testing()
file(GLOB TEST_SOURCES "tests/test_*.cc")
foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} vde)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

if(VDE_FUZZ)
    file(GLOB FUZZ_SOURCES "fuzz/*_fuzzer.cc")
    foreach(fuzz_src ${FUZZ_SOURCES})
        get_filename_component(fuzz_name ${fuzz_src} NAME_WE)
        add_executable(${fuzz_name} ${fuzz_src})
        target_link_libraries(${fuzz_name} vde)
        if(DEFINED ENV{LIB_FUZZING_ENGINE})
            target_link_libraries(${fuzz_name} $ENV{LIB_FUZZING_ENGINE})
        else()
            target_compile_options(${fuzz_name} PRIVATE -fsanitize=fuzzer)
            target_link_options(${fuzz_name} PRIVATE -fsanitize=fuzzer)
        endif()
    endforeach()
endif()
