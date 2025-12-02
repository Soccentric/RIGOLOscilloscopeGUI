cmake_minimum_required(VERSION 3.16)

project(RIGOLOscilloscopeGUI VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "C:\\Qt\\6.10.1\\mingw_64\\lib\\cmake")

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt Configuration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Gui 
    Widgets 
    Network 
    Charts 
    OpenGLWidgets
    OpenGL
)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/analysis
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
)

# Source files
set(COMMUNICATION_SOURCES
    src/communication/deviceconnection.cpp
    src/communication/waveformprovider.cpp
)

set(COMMUNICATION_HEADERS
    src/communication/deviceconnection.h
    src/communication/scpicommands.h
    src/communication/waveformprovider.h
)

set(CORE_SOURCES
    src/core/cursormanager.cpp
    src/core/measurementengine.cpp
    src/core/scopechannel.cpp
    src/core/scopesettings.cpp
)

set(CORE_HEADERS
    src/core/cursormanager.h
    src/core/measurementengine.h
    src/core/scopechannel.h
    src/core/scopesettings.h
)

set(UI_SOURCES
    src/ui/mainwindow.cpp
    src/ui/theme.cpp
    src/ui/waveformdisplay.cpp
    src/ui/channelcontrolpanel.cpp
    src/ui/triggerpanel.cpp
    src/ui/measurementpanel.cpp
    src/ui/connectiondialog.cpp
    src/ui/fftwindow.cpp
    src/ui/headerwidget.cpp
    src/ui/footerwidget.cpp
    src/ui/analogcontrolpanel.cpp
    src/ui/digitalcontrolpanel.cpp
)

set(UI_HEADERS
    src/ui/mainwindow.h
    src/ui/theme.h
    src/ui/waveformdisplay.h
    src/ui/waveformdisplay.h
    src/ui/channelcontrolpanel.h
    src/ui/triggerpanel.h
    src/ui/measurementpanel.h
    src/ui/connectiondialog.h
    src/ui/fftwindow.h
    src/ui/headerwidget.h
    src/ui/footerwidget.h
    src/ui/analogcontrolpanel.h
    src/ui/digitalcontrolpanel.h
)

set(ANALYSIS_SOURCES
    src/analysis/fftanalyzer.cpp
    src/analysis/protocoldecoder.cpp
)

set(ANALYSIS_HEADERS
    src/analysis/fftanalyzer.h
    src/analysis/protocoldecoder.h
)

set(UTILS_SOURCES
    src/utils/dataexporter.cpp
)

set(UTILS_HEADERS
    src/utils/dataexporter.h
)

# All sources
set(PROJECT_SOURCES
    src/main.cpp
    ${COMMUNICATION_SOURCES}
    ${COMMUNICATION_HEADERS}
    ${CORE_SOURCES}
    ${CORE_HEADERS}
    ${UI_SOURCES}
    ${UI_HEADERS}
    ${ANALYSIS_SOURCES}
    ${ANALYSIS_HEADERS}
    ${UTILS_SOURCES}
    ${UTILS_HEADERS}
)

# Create executable
qt_add_executable(RIGOLOscilloscopeGUI
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

# Link Qt libraries
target_link_libraries(RIGOLOscilloscopeGUI PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::Charts
    Qt6::OpenGLWidgets
    Qt6::OpenGL
)

# Platform-specific settings
if(WIN32)
    set_target_properties(RIGOLOscilloscopeGUI PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

if(APPLE)
    set_target_properties(RIGOLOscilloscopeGUI PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER com.rigol.oscilloscope
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    )
endif()

# Installation
install(TARGETS RIGOLOscilloscopeGUI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(RIGOLOscilloscopeGUI)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(RIGOLOscilloscopeGUI PRIVATE
        -Wall -Wextra -Wpedantic
    )
endif()

if(MSVC)
    target_compile_options(RIGOLOscilloscopeGUI PRIVATE
        /W4
    )
endif()

# Debug/Release configurations
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Print configuration summary
message(STATUS "")
message(STATUS "RIGOL Oscilloscope GUI Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Qt Version: ${Qt6_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "")