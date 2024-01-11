cmake_minimum_required(VERSION 3.15)
project(serwer)

set(CMAKE_CXX_STANDARD 14)



add_executable(serwer serwer1.2.cpp)
set_target_properties(serwer PROPERTIES COMPILE_FLAGS "-pthread")
target_link_libraries(serwer pthread)