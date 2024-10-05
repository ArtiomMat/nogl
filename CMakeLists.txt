cmake_minimum_required(VERSION 3.25)
project(nogl)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The cross platform files
file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB HEADERS "${CMAKE_SOURCE_DIR}/include/*.hpp")

# set(GLOBAL_LIBRARIES -lpng -lzlib)

# Add the platform source files and append them
if(WIN32)
  set(LIBRARIES -lopengl32 -lws2_32 -lole32 -lcomctl32 -lgdi32 -lwindowscodecs)
  file(GLOB PLATFORM_SOURCES "${CMAKE_SOURCE_DIR}/src/win32/*.cpp")
elseif(UNIX)
  set(LIBRARIES -lGL -lm)
  file(GLOB PLATFORM_SOURCES "${CMAKE_SOURCE_DIR}/src/unix/*.cpp")
endif()
list(APPEND SOURCES ${PLATFORM_SOURCES})

# Add an executable
add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_options(${PROJECT_NAME} PRIVATE 
  -Wall -Wextra -msse4 -mavx2
  $<$<CONFIG:Release>:-O3 -march=native -mtune=native -flto=auto -DNDEBUG>
  $<$<CONFIG:Debug>:-ggdb>
  ${PLATFORM_FLAGS})

target_link_options(${PROJECT_NAME} PRIVATE ${PLATFORM_LINKS})
# target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARIES} ${GLOBAL_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# target_precompile_headers(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include/nogl.hpp)

# Tests

# file(GLOB TEST_SOURCES "${CMAKE_SOURCE_DIR}/tests/*.cpp") 
# # Create a test executable
# add_executable(test_nogl ${TEST_SOURCES})

# # Link any necessary libraries (if applicable)
# target_link_options(test_nogl PRIVATE ${PLATFORM_LINKS})
# target_link_libraries(test_nogl PRIVATE ${LIBRARIES} ${GLOBAL_LIBRARIES})
# target_include_directories(test_nogl PRIVATE ${CMAKE_SOURCE_DIR}/include)

# # Enable testing
# enable_testing()

# # Add the test
# add_test(NAME Test COMMAND test_nogl)
