cmake_minimum_required(VERSION 3.13.4)
enable_testing()

project(simpledb)
option(SIMPLEDB_BUILD_TESTS "Build simpledb's unit tests" ON)

set(CMAKE_CXX_STANDARD 14)

file(GLOB SIMPLEDB_SOURCES src/**/*.cpp)

include_directories(include)
add_executable(simpledb ${SIMPLEDB_SOURCES})

if (SIMPLEDB_BUILD_TESTS)
  configure_file(cmake/test-packages.cmake.in test-packages/CMakeLists.txt)
  execute_process(COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test-packages )
  if(result)
    message(FATAL_ERROR "CMake step for test-packages failed: ${result}")
  endif()
  execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test-packages )
  if(result)
    message(FATAL_ERROR "Build step for test-packages failed: ${result}")
  endif()

  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  add_subdirectory(
    ${CMAKE_CURRENT_BINARY_DIR}/pipedcp-src
    ${CMAKE_CURRENT_BINARY_DIR}/pipedcp-build
    EXCLUDE_FROM_ALL
  )
  include_directories(build/pipedcp-src/include)

  add_subdirectory(
    ${CMAKE_CURRENT_BINARY_DIR}/googletest-src
    ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
    EXCLUDE_FROM_ALL
  )

  add_executable(db_test test/simpledb/db_test.cpp)
  target_sources(db_test PUBLIC src/simpledb/row.cpp)
  target_link_libraries(db_test gtest_main pipedcp)
  add_test(NAME db_test COMMAND db_test)
endif()
