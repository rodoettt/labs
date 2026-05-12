cmake_minimum_required(VERSION 3.16)
project(file_parser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Test)

add_library(file_model STATIC
    model/file_info.cpp
    model/parser.cpp
    model/file_model.cpp
    model/command_parser.cpp
)

target_include_directories(file_model PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/model
)

target_link_libraries(file_model
    Qt6::Core
)

add_executable(file_tests
    tests/test_file_model.cpp
)

target_include_directories(file_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/model
)

target_link_libraries(file_tests
    file_model
    Qt6::Core
    Qt6::Test
)

add_executable(file_app
    main.cpp
    view/mainwindow.cpp
    view/mainwindow.h
    model/parser_exceptions.h
    model/file_info.h
    model/file_model.h
    model/parser.h
    model/command_parser.h
)

target_include_directories(file_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/model
    ${CMAKE_CURRENT_SOURCE_DIR}/view
)

target_link_libraries(file_app
    file_model
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

enable_testing()
add_test(NAME FileModelTests COMMAND file_tests)