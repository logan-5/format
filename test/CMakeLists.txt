if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/catch.hpp)
    file(DOWNLOAD
         https://raw.githubusercontent.com/catchorg/Catch2/master/single_include/catch2/catch.hpp
         ${CMAKE_CURRENT_BINARY_DIR}/catch.hpp
         STATUS status
         LOG log)

    list(GET status 0 status_code)
    list(GET status 1 status_string)

    if(NOT status_code EQUAL 0)
        message(FATAL_ERROR "error downloading catch: ${status_string}"
                "${log}")
    endif()
endif()

set(source_files 
    format_error.cpp
    format_to_n.cpp 
    formatters.cpp 
    formatters_static.cpp 
    formatted_size.cpp 
    hello_world.cpp 
    invalid.cpp
    locale.cpp
    make_format_args.cpp 
    output.cpp 
    representable_as_char.cpp 
    small_args.cpp
    std_examples.cpp 
    test.cpp
)

set(SANITIZER_FLAGS "")

option(ASAN "ASAN" OFF)
if (ASAN)
    message("-- enabling address sanitizer")
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        set(SANITIZER_FLAGS "${SANITIZER_FLAGS}address,")
    else (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        message("address sanitizer not supported for this compiler")
    endif (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
else (ASAN)
    message("-- not enabling address sanitizer")
endif (ASAN)

option(UBSAN "UBSAN" OFF)
if (UBSAN)
    message("-- enabling undefined behavior sanitizer")
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    set(SANITIZER_FLAGS "${SANITIZER_FLAGS}undefined,")
    else (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        message("undefined behavior sanitizer not supported for this compiler")
    endif (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
else (UBSAN)
    message("-- not enabling undefined behavior sanitizer")
endif (UBSAN)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=${SANITIZER_FLAGS}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -pedantic -Wno-missing-field-initializers -Wconversion")
add_executable(format_test ${source_files})
target_link_libraries(format_test PUBLIC format)
target_include_directories(format_test PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
set_property(TARGET format_test PROPERTY CXX_STANDARD 17)

add_test(NAME test COMMAND format_test)