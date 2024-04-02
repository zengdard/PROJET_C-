cmake_minimum_required(VERSION 3.22)
project(projetcppbanque)

set(CMAKE_CXX_STANDARD 17)

find_package(mariadbcpp)

include_directories("/usr/local/include/mariadb")
link_directories("/usr/local/lib/mariadb")
include_directories("/usr/include/mariadb") #path to include folder
add_library(mariadbcpp STATIC IMPORTED
        backOffice/backOffice.cpp
        backOffice/backOffice.h)
set_property(TARGET mariadbcpp PROPERTY IMPORTED_LOCATION "/usr/lib/libmariadbcpp.so")

add_executable(projetcppbanque main.cpp
        frontOffice/user.cpp
        frontOffice/user.h
        bankingApp/bankingApp.cpp
        bankingApp/bankingApp.h
        backOffice/backOffice.h
        backOffice/backOffice.cpp)

target_link_libraries(projetcppbanque mariadbcpp)