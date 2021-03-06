cmake_minimum_required(VERSION 3.6)
project(Nodoka)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 2)

SET(CPACK_GENERATOR "DEB")
SET(CPACK_BINARY_DEB "ON")
SET(CPACK_DEBIAN_PACKAGE_NAME "nodoka") #required
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Mistlight Oriroris") #required
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)


set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/libs/quazip-0.7.2/")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)


set(CMAKE_THREAD_LIBS_INIT ON)

# auto generate uic
set(CMAKE_AUTOUIC ON)

# ensure resources are loaded
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)

# resource file
set (RESOURCES icons.qrc)

# library related things
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Threads REQUIRED)
find_package(QuaZip REQUIRED)

file(GLOB zlib_SOURCES
        "${CMAKE_SOURCE_DIR}/libs/zlib123/*.h"
        "${CMAKE_SOURCE_DIR}/libs/zlib123/*.cpp"
        "${CMAKE_SOURCE_DIR}/libs/zlib123/*.c"
        )
add_library(zlib ${zlib_SOURCES})


if(APPLE)
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/libvlc/mac)
elseif(WIN32)
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/libvlc/win32)
    set(PLATFORM_LINKS "ntdll")
endif()

FILE(GLOB_RECURSE LibFiles "src/*.h" "src/*.hpp")
add_custom_target(headers SOURCES ${LibFiles})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCE_FILES src/main.cpp src/core/ConcretePlayer.cpp
        src/core/ConcretePlayer.h src/ui-element/MainWindow.cpp src/ui-element/MainWindow.h src/ui-element/SettingsForm.cpp
        src/ui-element/SettingsForm.h src/core/AudiobookScan.cpp src/model/Directory.cpp src/model/Directory.h
        src/core/DatabaseConnect.cpp src/core/DatabaseConnect.h src/core/NodokaApp.cpp src/core/NodokaApp.h
        src/event-handler/DirectoryHandler.cpp src/event-handler/DirectoryHandler.h src/model/Audiobook.cpp
        src/model/Audiobook.h src/model/AudiobookRecord.cpp src/model/AudiobookRecord.h src/model/AudiobookFile.cpp
        src/model/AudiobookFile.h src/model/AudiobookFileRecord.cpp src/model/AudiobookFileRecord.h src/model/FileDisplayModel.cpp
        src/model/FileDisplayModel.h src/proxy-objects/AudiobookFileProxy.cpp src/proxy-objects/AudiobookFileProxy.h src/event-handler/PlayerEventHandler.cpp
        src/event-handler/PlayerEventHandler.h src/model/MediaProperty.cpp src/model/MediaProperty.h src/core/Setting.cpp src/core/Setting.h
        src/simple-lib/RunGuard.cpp src/simple-lib/RunGuard.h src/model/AudiobookListDelegate.cpp src/model/AudiobookListDelegate.h
        src/core/Util.cpp src/core/Util.h src/proxy-objects/ProxyManager.cpp src/proxy-objects/ProxyManager.h
        src/proxy-objects/AudiobookProxy.cpp src/proxy-objects/AudiobookProxy.h src/event-handler/AudiobookListViewHandler.cpp
        src/event-handler/AudiobookListViewHandler.h src/core/ScanPlayer.cpp src/core/ScanPlayer.h
        src/core/tasks/ScanDirectoryTask.cpp src/core/tasks/ScanDirectoryTask.h
        src/core/tasks/PlayerScanTask.cpp src/core/tasks/PlayerScanTask.h src/core/tasks/InitialScanTask.cpp
        src/core/tasks/InitialScanTask.h src/core/tasks/ChecksumTask.cpp src/core/tasks/ChecksumTask.h
        src/event-handler/AudiobookCollectionHandler.cpp src/event-handler/AudiobookCollectionHandler.h src/event-handler/FileListViewHandler.cpp
        src/event-handler/FileListViewHandler.h src/core/directory-element-scan/AbstractElementScanner.cpp
        src/core/directory-element-scan/AbstractElementScanner.h
        src/core/directory-element-scan/AddRemoveVisitor.cpp src/core/directory-element-scan/AddRemoveVisitor.h
        )
if(NOT CMAKE_BUILD_TYPE MATCHES Debug)
    set(WIN32_ENABLED WIN32 )
endif()

add_executable(Nodoka ${WIN32_ENABLED} ${SOURCE_FILES} ${RESOURCES})

target_link_libraries(Nodoka Qt5::Widgets Qt5::Sql vlc vlccore Threads::Threads zlib ${PLATFORM_LINKS})

install(TARGETS Nodoka DESTINATION bin)
include(CPack)



