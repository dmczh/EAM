cmake_minimum_required(VERSION 3.16)

project(eam-client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Network)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(QT_DEBUG)
    add_compile_options(-g -O0 -Wall -Wextra)
    message(STATUS "Building in DEBUG mode")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(QT_NO_DEBUG)
    add_compile_options(-O2 -DNDEBUG)
    message(STATUS "Building in RELEASE mode")
endif()


set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(eam-client
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        httpclient.h httpclient.cpp
        configmanager.h configmanager.cpp
        logger.h logger.cpp

        logindialog.h logindialog.cpp logindialog.ui
        backgroundimg.qrc
        mainpagewidget.h mainpagewidget.cpp mainpagewidget.ui
        tokenmanager.h tokenmanager.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET eam-client APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(eam-client SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(eam-client
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(eam-client PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(eam-client PRIVATE Qt6::Network)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.eam-client)
endif()
set_target_properties(eam-client PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS eam-client
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(eam-client)
endif()

configure_file(
    ${CMAKE_SOURCE_DIR}/config.json
    ${CMAKE_BINARY_DIR}/bin/config.json
    COPYONLY
)

install(FILES ${CMAKE_SOURCE_DIR}/config.json
DESTINATION ${CMAKE_INSTALL_BINDIR}
)

message(STATUS "========================================")
message(STATUS "Build Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Output dir: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "========================================")