cmake_minimum_required(VERSION 2.8)
project(test)

set(OpenCV_DIR "F:/opencv-3.4.5/mingw-build/")
SET(DIRENT_DIR "../dirent/include/")

set(BOOST_ROOT F:/boost_1_71_0/)
set(BOOST_INCLUDEDIR F:/boost_1_71_0/)
set(BOOST_LIBRARYDIR F:/boost_1_71_0/stage/lib)

set (Boost_USE_STATIC_LIBS ON) # enable dynamic linking
set (Boost_USE_MULTITHREAD ON)  # enable multithreading
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_COMPILER "-mgw81")
set(Boost_ARCHITECTURE "-x64")

find_package(Boost
        1.71.0
        REQUIRED COMPONENTS
        filesystem iostreams thread date_time system)

set(GNU_INCLUDE_DIRS "G:/ClionProjects/gnuplot-iostream/")
find_package(OpenCV REQUIRED)

if(NOT WIN32)
ADD_DEFINITIONS("-std=c++0x -O3")
endif(NOT WIN32)

include_directories(src ${DIRENT_DIR} ${Boost_INCLUDE_DIR} ${GNU_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR})
add_executable( KCF src/fhog.cpp src/kcftracker.cpp src/runtracker.cpp )
target_link_libraries( KCF ${OpenCV_LIBS})

add_executable( testKCF src/testRunTracker.cpp src/fhog.cpp src/kcftracker.cpp )
target_link_libraries( testKCF ${OpenCV_LIBS} ${Boost_LIBRARIES})

add_executable( testgnu src/example.cpp )
target_link_libraries( testgnu ${Boost_LIBRARIES})

