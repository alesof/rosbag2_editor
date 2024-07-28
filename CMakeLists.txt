cmake_minimum_required(VERSION 3.5)

project(ros2bag_editor VERSION 1.1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

find_package(ament_cmake REQUIRED)
find_package(rosbag2_cpp REQUIRED)

add_subdirectory(rosx_introspection)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    src/info_dialog.ui
    src/info_dialog.h
    src/info_dialog.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ros2bag_editor
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resource_icons.qrc
        icons/chat-bubble-xmark.svg icons/chat-minus-in.svg icons/chat-plus-in.svg icons/floppy-disk.svg icons/import.svg icons/upload.svg icons/export_white.png
        include/rosbag_parser.h
    )
else()
    add_executable(ros2bag_editor
        ${PROJECT_SOURCES}
        resource_icons.qrc
    )
endif()

target_include_directories(ros2bag_editor PRIVATE ${rosbag2_cpp_INCLUDE_DIRS})
ament_target_dependencies(ros2bag_editor PUBLIC rosbag2_cpp)
target_link_libraries(ros2bag_editor PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    rosx_introspection
)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ros2bag_editor)
endif()
set_target_properties(ros2bag_editor PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ros2bag_editor
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${CMAKE_BINARY_DIR}/rosx_introspection/ DESTINATION ${CMAKE_INSTALL_PREFIX}/rosx_introspection)

# CPack configuration for deb package
set(CPACK_PACKAGE_NAME "ros2bag_editor")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_CONTACT "alesof <alessandro.sofia@unina.it>")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_INSTALL_PREFIX "/usr/local")

include(CPack)