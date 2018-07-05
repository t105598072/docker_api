cmake_minimum_required(VERSION 3.5)
project(example)
set(LIBDOCKER_DIR lib/libdocker)

# Add subdirectory to compile the static library
add_subdirectory(${LIBDOCKER_DIR})
add_executable(example example.c)

# Make sure CMake knows where the header files are
target_include_directories(
    ${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/${LIBDOCKER_DIR}/${DOCKER_INCLUDE_DIR}
)

# Link in the static library
target_link_libraries(
    ${PROJECT_NAME}
    docker
    
)
