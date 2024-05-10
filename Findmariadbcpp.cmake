find_path(MARIADBCPP_INCLUDE_DIR mysql_driver.h
        PATHS /usr/include/mariadb /usr/local/include/mysql
        PATH_SUFFIXES mariadbcpp mariadb
)

find_library(MARIADBCPP_LIBRARY
        NAMES mariadbcpp
        PATHS /usr/lib /usr/local/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Mariadbcpp DEFAULT_MSG
        MARIADBCPP_LIBRARY MARIADBCPP_INCLUDE_DIR
)

if(MARIADBCPP_FOUND)
    set(MARIADBCPP_INCLUDE_DIRS ${MARIADBCPP_INCLUDE_DIR})
    set(MARIADBCPP_LIBRARIES ${MARIADBCPP_LIBRARY})
else()
    set(MARIADBCPP_INCLUDE_DIRS)
    set(MARIADBCPP_LIBRARIES)
endif()

mark_as_advanced(MARIADBCPP_INCLUDE_DIR MARIADBCPP_LIBRARY)
