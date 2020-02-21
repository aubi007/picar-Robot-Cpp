project(RobotApp)

#**************************************************************************************************
# General cMake settings
#**************************************************************************************************
cmake_minimum_required(VERSION 3.5)

#**************************************************************************************************
# Find Package **************************************************************************************************
find_package(OpenCV REQUIRED)
MESSAGE( " *** OpenCV_INCLUDE_DIRS : " ${OpenCV_INCLUDE_DIRS} )
MESSAGE( " *** OpenCV_LIB_DIRS : " ${OpenCV_LIB_DIRS} )
MESSAGE( " *** OpenCV_LIBS : " ${OpenCV_LIBS} )

find_package(PythonLibs 3 REQUIRED)
MESSAGE( " *** PYTHON_INCLUDE_DIRS : " ${PYTHON_INCLUDE_DIRS} )
MESSAGE( " *** PYTHON_LIBRARIES : " ${PYTHON_LIBRARIES} )

find_package(pybind11 CONFIG REQUIRED)

#**************************************************************************************************
# Include **************************************************************************************************
include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS}/opencv4)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${pybind11_INCLUDE_DIR})

#**************************************************************************************************
# Set variable **************************************************************************************************
SET(SOURCES
  ${CMAKE_CURRENT_DIR}RobotApp.cpp
  ${CMAKE_CURRENT_DIR}Stream.cpp
)

#**************************************************************************************************
# Set compiler **************************************************************************************************
SET(CMAKE_CXX_FLAGS "-std=c++11 -Wall -O3 -fPIC")

#**************************************************************************************************
# Linker **************************************************************************************************
LINK_DIRECTORIES(
  ${OpenCV_LIB_DIR}
)

#**************************************************************************************************
# Make configuration
#**************************************************************************************************
add_library(RobotApp SHARED ${SOURCES})
target_link_libraries(RobotApp ${PYTHON_LIBRARIES} ${OpenCV_LIBS})
SET_TARGET_PROPERTIES(RobotApp PROPERTIES PREFIX "")

install(TARGETS RobotApp DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../RobotWeb)