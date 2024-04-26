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

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        newprojectdialog.ui
        newprojectdialog.h
        newprojectdialog.cpp

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ros2bag_editor
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resource_icons.qrc
        icons/chat-bubble-xmark.svg icons/chat-minus-in.svg icons/chat-plus-in.svg icons/floppy-disk.svg icons/import.svg icons/upload.svg
        newprojectdialog.ui newprojectdialog.h newprojectdialog.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ros2bag_editor APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ros2bag_editor SHARED
            ${PROJECT_SOURCES}
            resource_icons.qrc
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ros2bag_editor
            ${PROJECT_SOURCES}
            resource_icons.qrc
        )
    endif()
endif()

target_include_directories(ros2bag_editor PRIVATE ${rosbag2_cpp_INCLUDE_DIRS})
ament_target_dependencies(ros2bag_editor PUBLIC rosbag2_cpp)
target_link_libraries(ros2bag_editor PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
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
