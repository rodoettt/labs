cmake_minimum_required(VERSION 3.16)
project(FileManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Поиск Qt
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
if(NOT Qt6_FOUND)
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets Test)
    set(QT_VERSION_MAJOR 5)
    set(QT_TESTLIB Qt5::Test)
else()
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets Test)
    set(QT_VERSION_MAJOR 6)
    set(QT_TESTLIB Qt6::Test)
endif()

# Общие исходники модели
set(MODEL_SOURCES
    model/File.cpp
    model/FileManagerModel.cpp
)

set(MODEL_HEADERS
    model/File.h
    model/FileManagerModel.h
)

# Основное приложение
set(APP_SOURCES
    main.cpp
    ${MODEL_SOURCES}
    view/MainWindow.cpp
    view/AddFileDialog.cpp
    view/ImportWindow.cpp
)

set(APP_HEADERS
    ${MODEL_HEADERS}
    view/MainWindow.h
    view/AddFileDialog.h
    view/ImportWindow.h
)

# Исполняемый файл приложения
add_executable(${PROJECT_NAME} ${APP_SOURCES} ${APP_HEADERS})

if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(${PROJECT_NAME}
        Qt6::Core
        Qt6::Widgets
    )
else()
    target_link_libraries(${PROJECT_NAME}
        Qt5::Core
        Qt5::Widgets
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Настройка для macOS
if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
    )
endif()

# ============= ТЕСТЫ =============
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    enable_testing()

    # Тестовый исполняемый файл
    add_executable(test_model
        tests/test_model.cpp
        ${MODEL_SOURCES}
    )

    target_link_libraries(test_model
        ${QT_TESTLIB}
    )

    target_include_directories(test_model PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    # Добавляем тест в CTest
    add_test(NAME FileManagerModelTest COMMAND test_model)

    message(STATUS "Tests enabled. Build with 'make test_model'")
endif()