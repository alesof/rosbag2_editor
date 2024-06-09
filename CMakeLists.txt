cmake_minimum_required(VERSION 3.5)

project(ros2bag_editor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

find_package(ament_cmake REQUIRED)
find_package(rosbag2_cpp REQUIRED)
find_package(geometry_msgs REQUIRED)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/mainwindow.ui
        src/newprojectdialog.ui
        src/newprojectdialog.h
        src/newprojectdialog.cpp

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ros2bag_editor
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resource_icons.qrc
        icons/chat-bubble-xmark.svg icons/chat-minus-in.svg icons/chat-plus-in.svg icons/floppy-disk.svg icons/import.svg icons/upload.svg
        newprojectdialog.ui newprojectdialog.h newprojectdialog.cpp
    )

else()
    add_executable(ros2bag_editor
        ${PROJECT_SOURCES}
        resource_icons.qrc
    )
endif()

target_include_directories(ros2bag_editor PRIVATE ${rosbag2_cpp_INCLUDE_DIRS})
ament_target_dependencies(ros2bag_editor PUBLIC rosbag2_cpp geometry_msgs)
target_link_libraries(ros2bag_editor PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
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

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ros2bag_editor)
endif()
