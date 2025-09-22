cmake_minimum_required(VERSION 3.16)
project(skaScreen LANGUAGES CXX)

#解决奇数个数中文在msvc上无法编译utf-8不带bom的代码文件
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

include(qt.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(3rd/tyme4cpp/tyme)

#设置qt6路径
set(CMAKE_PREFIX_PATH "C:/Qt/6.7.2/msvc2019_64/lib/cmake")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Core
        Gui
        Widgets
        Network
        Charts
)
qt_standard_project_setup()

set(PROJECT_SOURCES
    main.cpp

    #pages
    src/pages/home/home.cpp
    src/pages/home/home.h
    src/pages/home/home.ui

    #core
    src/setting.h
    src/skaScreen.h
    src/skaScreen.cpp
    src/core/data/data.hpp
    src/core/data/data.cpp
    src/core/data/data_fetch/data_fetch.hpp
    src/core/data/data_fetch/data_fetch.cpp

    #utils
    src/utils/blur.h
    src/utils/img.h

)

qt_add_resources(RESOURCE_FILES src/res/res.qrc)
qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${RESOURCE_FILES})

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network
        Qt::Charts
        tyme
)

