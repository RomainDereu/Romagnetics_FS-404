cmake_minimum_required(VERSION 3.16)
project(FS-404 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Qt dependencies ----
find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort)

# ---- Project sources ----
set(PROJECT_SOURCES
    main.cpp
    widget.cpp
    widget.h
    note_interface.h
    note_interface.cpp
    note_interface.ui
    widget.ui
    serial.cpp
    midi_note.h
    midi_note.cpp
)

qt_add_executable(FS-404
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_link_libraries(FS-404 PRIVATE Qt6::Widgets Qt6::SerialPort)

set_target_properties(FS-404 PROPERTIES WIN32_EXECUTABLE TRUE)

# ---- Output paths ----
set(DEPLOY_DIR "${CMAKE_SOURCE_DIR}/deploy/FS-404")

# Put exe directly in build folder (not /Release subdir)
set_target_properties(FS-404 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

qt_finalize_executable(FS-404)

# ---- Automatic deployment ----
if (WIN32)
  # Path to windeployqt
  set(WINDEPLOYQT_EXE "C:/Qt/6.8.0/mingw_64/bin/windeployqt.exe")

  add_custom_command(
    TARGET FS-404 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${DEPLOY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${DEPLOY_DIR}"
    # Copy exe
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:FS-404>" "${DEPLOY_DIR}/"
    # Run windeployqt to populate DLLs beside the exe
    COMMAND "${WINDEPLOYQT_EXE}" --release --no-translations --compiler-runtime "${DEPLOY_DIR}/FS-404.exe"
    COMMENT "Deploying FS-404 automatically to ${DEPLOY_DIR}"
  )
endif()

# ---- Optional: Create ZIP package automatically ----
add_custom_target(package_fs404 ALL
  DEPENDS FS-404
  COMMAND "${CMAKE_COMMAND}" -E tar "cfv" "${CMAKE_SOURCE_DIR}/deploy/FS-404.zip" --format=zip "FS-404"
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/deploy"
  COMMENT "Packaging FS-404 into deploy/FS-404.zip"
)

# Include headers in build
target_include_directories(FS-404 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
