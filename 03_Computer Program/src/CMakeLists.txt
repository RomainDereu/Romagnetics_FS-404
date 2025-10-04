cmake_minimum_required(VERSION 3.16)
project(FS-404 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Qt5/Qt6 fallback (Widgets + SerialPort for both) ----
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets SerialPort)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets SerialPort)

# ---- Sources (no duplicates) ----
set(PROJECT_SOURCES
  main.cpp
  widget.cpp
  widget.h
  widget.ui
  note_interface.h
  note_interface.cpp
  note_interface.ui
  midi_note.h
  midi_note.cpp
  serial.cpp
)

# ---- Targets (keep your OS options as they were) ----
if (QT_VERSION_MAJOR GREATER_EQUAL 6)
  qt_add_executable(FS-404
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
  )
else()
  if (ANDROID)
    add_library(FS-404 SHARED
      ${PROJECT_SOURCES}
    )
    # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
  else()
    add_executable(FS-404
      ${PROJECT_SOURCES}
    )
  endif()
endif()

# ---- Link Qt libs for whichever major version we found ----
target_link_libraries(FS-404 PRIVATE
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::SerialPort
)

# ---- Bundle/app properties (same as yours) ----
# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If developing for iOS/macOS, consider setting a fixed bundle id manually.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.FS-404)
endif()

set_target_properties(FS-404 PROPERTIES
  ${BUNDLE_ID_OPTION}
  MACOSX_BUNDLE_BUNDLE_VERSION           ${PROJECT_VERSION}
  MACOSX_BUNDLE_SHORT_VERSION_STRING     ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
  MACOSX_BUNDLE                          TRUE
  WIN32_EXECUTABLE                       TRUE
)

include(GNUInstallDirs)
install(TARGETS FS-404
  BUNDLE  DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if (QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(FS-404)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
