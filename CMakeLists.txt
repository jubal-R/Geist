cmake_minimum_required(VERSION 3.14)

project(
    Geist
    VERSION 0.1.0
    LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(NO_CACHEGEN "NO_CACHEGEN")
endif()

include(FeatureSummary)

find_package(
    QT NAMES Qt6 Qt5
    COMPONENTS Core
    REQUIRED
)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)

include_directories(src)

set(PROJECT_SOURCES
    src/files.cpp
    src/highlighter.cpp
    src/files.h
    src/highlighter.h
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.ui
    src/mainwindow.h
    src/search.cpp
    src/utils/conversion.cpp
    src/utils/templates.cpp
    src/utils/conversion.h
    src/utils/templates.h
    src/widgets/geisttextedit.cpp
    src/widgets/geisttextedit.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
    )
else()
    add_executable(${PROJECT_NAME})
endif()

target_sources(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
