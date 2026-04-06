cmake_minimum_required(VERSION 3.16)

project(STLViewer VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Let CMake run MOC and RCC automatically
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Export compile commands for IDE tooling (clangd, IntelliSense)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

find_package(OpenGL REQUIRED)
find_package(Qt6 6.0 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets)

include(GNUInstallDirs)

add_executable(stlviewer
    src/AxisGLWidget.cpp
    src/AxisGroupBox.cpp
    src/DimensionsGroupBox.cpp
    src/GeometryEngine.cpp
    src/GLWidget.cpp
    src/GuiIface.cpp
    src/main.cpp
    src/MainWindow.cpp
    src/MeshInformationGroupBox.cpp
    src/PropertiesGroupBox.cpp
    src/RenderWidget.cpp
    src/SettingsDialog.cpp
    src/STLFile.cpp
    resources/resources.qrc
    resources/shaders.qrc
)

target_link_libraries(stlviewer PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    OpenGL::GL
)

target_include_directories(stlviewer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_include_directories(stlviewer PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS stlviewer
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(UNIX AND NOT APPLE)
    install(FILES cmake/stlviewer.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
    install(FILES resources/icons/stlviewer_48.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/48x48/apps
        RENAME stlviewer.png)
    install(FILES resources/icons/stlviewer_256.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps
        RENAME stlviewer.png)
    install(FILES STLViewer.ico
        DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps)
endif()

if(WIN32)
    install(FILES STLViewer.ico DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

include(cmake/CPackConfig.cmake)
