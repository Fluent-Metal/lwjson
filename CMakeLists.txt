cmake_minimum_required(VERSION 3.22)

# Setup project
project(LwLibPROJECT)

if(NOT PROJECT_IS_TOP_LEVEL)
    add_subdirectory(lwjson)
else()
    add_executable(${PROJECT_NAME})
    target_sources(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/dev/main.c
        ${CMAKE_CURRENT_LIST_DIR}/test/test.c
        ${CMAKE_CURRENT_LIST_DIR}/examples/example_minimal.c
        ${CMAKE_CURRENT_LIST_DIR}/examples/example_traverse.c
        ${CMAKE_CURRENT_LIST_DIR}/examples/example_stream.c
        ${CMAKE_CURRENT_LIST_DIR}/trial_env/trial_run.c
    )
    target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/dev
    )

    # Add subdir with lwjson and link to project
    set(LWJSON_OPTS_FILE ${CMAKE_CURRENT_LIST_DIR}/dev/lwjson_opts.h)
    add_subdirectory(lwjson)
    target_link_libraries(${PROJECT_NAME} lwjson)
    target_link_libraries(${PROJECT_NAME} lwjson_debug)

    # Add compile options to the library, which will propagate options to executable through public link
    target_compile_definitions(lwjson PUBLIC WIN32 _DEBUG CONSOLE LWJSON_DEV)
    target_compile_options(lwjson PUBLIC -Wall -Wextra -Wpedantic)
endif()