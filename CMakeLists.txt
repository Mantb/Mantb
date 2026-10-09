cmake_minimum_required(VERSION 3.10)
project(ImageSearch)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCV (required for SIFT and image loading)
find_package(OpenCV REQUIRED)

# Find Eigen3 (required for linear algebra / vector operations)
# It's a header only library.
find_package(Eigen3 REQUIRED)

# Include directories
include_directories(
    include
    ${OpenCV_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIR}
)

# Source files
set(SOURCES
    src/main.cpp
    src/image_utils.cpp
    src/search_index.cpp
    src/lsh.cpp
    src/hypercube.cpp
    src/ivf.cpp
)

# Executable
add_executable(image_search ${SOURCES})

# Link libraries
target_link_libraries(image_search ${OpenCV_LIBS} Eigen3::Eigen)
