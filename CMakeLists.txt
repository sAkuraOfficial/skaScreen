cmake_minimum_required(VERSION 3.16)
project(skaScreen LANGUAGES CXX)

include(qt.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Core
        Gui
        Widgets
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
)

