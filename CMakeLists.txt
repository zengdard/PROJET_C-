cmake_minimum_required(VERSION 3.18)

project(retalbank)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS 
        Core
        Gui
        Widgets
        REQUIRED)

find_package(mariadbcpp)
find_package(OpenSSL REQUIRED)

include_directories("/usr/local/include/mariadb")
link_directories("/usr/local/lib/mariadb")
include_directories("/usr/include/mariadb") # Path to include folderadd_library(mariadbcpp STATIC IMPORTED)
set_property(TARGET mariadbcpp PROPERTY IMPORTED_LOCATION "/usr/lib/libmariadbcpp.so")

include_directories(${OPENSSL_INCLUDE_DIR})

add_executable(retalbank src/main.cpp
        src/db/DatabaseManager.cpp
        include/db/DatabaseManager.h
        src/ui/front_office/AccountView.cpp
        include/ui/front_office/AccountView.h
        src/ui/front_office/WireView.cpp
        include/ui/front_office/WireView.h
        src/ui/front_office/HistoryView.cpp
        include/ui/front_office/HistoryView.h
        src/ui/common/Card.cpp
        include/ui/common/Card.h
        src/ui/common/MainWindow.cpp
        include/ui/common/MainWindow.h
        src/queries/UserQueries.cpp
        include/queries/UserQueries.h
        src/queries/AccountQueries.cpp
        include/queries/AccountQueries.h
        src/queries/TransactionQueries.cpp
        include/queries/TransactionQueries.h
        src/queries/APIQueries.cpp
        include/queries/APIQueries.h
        src/ui/back_office/UserManagementView.cpp
        include/ui/back_office/UserManagementView.h
        src/ui/back_office/AccountCreationView.cpp
        include/ui/back_office/AccountCreationView.h
        src/models/UserModel.cpp
        include/models/UserModel.h
        src/models/AccountModel.cpp
        include/models/AccountModel.h
        src/models/TransactionModel.cpp
        include/models/TransactionModel.h
        src/ui/LoginView.cpp
        include/ui/LoginView.h
        ressources/ressources.qrc
        src/ui/back_office/UserEditDialog.cpp
        include/ui/back_office/UserEditDialog.h
        src/ui/front_office/HomeView.cpp
        include/ui/front_office/HomeView.h
        src/ui/back_office/AllHistoricView.cpp
        include/ui/back_office/AllHistoricView.h
        src/ui/back_office/AccountWireView.cpp
        include/ui/back_office/AccountWireView.h)

target_link_libraries(retalbank
        ${OPENSSL_CRYPTO_LIBRARIES}        
        mariadbcpp
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()
