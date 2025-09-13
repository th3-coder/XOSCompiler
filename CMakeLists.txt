cmake_minimum_required(VERSION 3.10)
project(filterImg)

# Find OpenCV 4.x
find_package(OpenCV 4 REQUIRED)

# Add your source file
add_executable( filterImg
filterImg.cpp
)

# Include OpenCV headers
target_include_directories(filterImg PRIVATE ${OpenCV_INCLUDE_DIRS})

# Link OpenCV libraries
target_link_libraries(filterImg PRIVATE ${OpenCV_LIBS})
