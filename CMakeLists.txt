cmake_minimum_required(VERSION 2.8)
project(driver)

## ECE650 Specific configuration start here
## Project specific configurations go here

# enable c++11 support
set (CMAKE_CXX_FLAGS "-std=c++11 -Wall ${CMAKE_CXX_FLAGS}")
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/run)

# create the main executable
## add additional .cpp files if needed
add_executable(spath spath.cpp )
add_executable(driver driver.cpp )
add_executable(rgen rgen.cpp)

install(PROGRAMS graph.py DESTINATION bin)
install(TARGETS spath driver rgen RUNTIME DESTINATION bin)
