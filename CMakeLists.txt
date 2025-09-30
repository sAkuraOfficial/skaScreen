cmake_minimum_required(VERSION 3.20)

# Avoid dependency scanning helpers like clang-scan-deps which are not shipped with
# certain Emscripten distributions (e.g. the Qt-provided toolchain). Fall back to
# compiler-driven depfiles so Ninja builds succeed.
set(CMAKE_DEPENDS_USE_COMPILER OFF)
set(CMAKE_C_DEPENDS_USE_COMPILER OFF)
set(CMAKE_CXX_DEPENDS_USE_COMPILER OFF)

project(skaScreen LANGUAGES CXX)

#解决奇数个数中文在msvc上无法编译utf-8不带bom的代码文件
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

include(qt.cmake)
if(EMSCRIPTEN)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s GL_ENABLE_GET_PROC_ADDRESS")
endif()
if(EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_PTHREADS=1 -matomics -mbulk-memory")
endif()
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
    src/pages/home/SystemChartWidget.cpp
    src/pages/home/SystemChartWidget.h

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

