cmake_minimum_required(VERSION 3.16)
project(CVEdit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 COMPONENTS Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(OpenCV REQUIRED)

qt_add_executable(CVEdit
        src/main.cpp
        src/main_window.h
        src/main_window.cpp
        src/main_window_helpers.cpp
        src/utils.h
        src/utils.cpp
        src/video_source.h
        src/video_source.cpp
        resources/icons.qrc

        src/filters/filter.h
        src/filters/filter.cpp
        src/filters/filter_grayscale.h
        src/filters/filter_blur.h
        src/filters/filter_canny.h
        src/filters/filter_shake.h
        src/filters/filter_sort.h
        src/filters/filter_sort.cpp
        src/filters/filter_kuwahara.h
        src/filters/filter_bloom.h
)

target_link_libraries(CVEdit PRIVATE Qt6::Widgets ${OpenCV_LIBS})