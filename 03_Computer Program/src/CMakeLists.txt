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
    MACOSX_BUNDLE
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_link_libraries(FS-404 PRIVATE Qt6::Widgets Qt6::SerialPort)

# Only has effect on Windows (creates a GUI subsystem exe)
set_target_properties(FS-404 PROPERTIES WIN32_EXECUTABLE TRUE)

# ---- Output / deploy paths (BUILD TREE, not source) ----
set(DEPLOY_ROOT "${CMAKE_BINARY_DIR}/deploy")
set(DEPLOY_DIR  "${DEPLOY_ROOT}/FS-404")

# Put exe directly in build folder (not /Release subdir)
set_target_properties(FS-404 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

qt_finalize_executable(FS-404)

# ---- Copy built binary into deploy dir on ALL platforms ----
# This ensures DEPLOY_DIR exists and contains FS-404 before packaging.
add_custom_command(
  TARGET FS-404 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E rm -rf "${DEPLOY_DIR}"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${DEPLOY_DIR}"
  COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:FS-404>" "${DEPLOY_DIR}/"
  COMMENT "Copying FS-404 into ${DEPLOY_DIR}"
)

# ---- Windows-specific Qt deployment ----
if (WIN32)
  # Path to windeployqt (your original path preserved)
  set(WINDEPLOYQT_EXE "C:/Qt/6.8.0/mingw_64/bin/windeployqt.exe")

  add_custom_command(
    TARGET FS-404 POST_BUILD
    COMMAND "${WINDEPLOYQT_EXE}"
            --release
            --no-translations
            --compiler-runtime
            "${DEPLOY_DIR}/FS-404.exe"
    COMMENT "Running windeployqt into ${DEPLOY_DIR}"
  )
endif()

# ---- Create ZIP package automatically on ALL platforms ----
set(DEPLOY_ZIP "${DEPLOY_ROOT}/FS-404.zip")

add_custom_target(package_fs404 ALL
  DEPENDS FS-404
  # Make sure deploy root exists
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${DEPLOY_ROOT}"
  # Tar the FS-404 folder in DEPLOY_ROOT into FS-404.zip
  COMMAND "${CMAKE_COMMAND}" -E tar "cfv" "${DEPLOY_ZIP}" --format=zip "FS-404"
  WORKING_DIRECTORY "${DEPLOY_ROOT}"
  COMMENT "Packaging FS-404 into ${DEPLOY_ZIP}"
)

# Include headers in build
target_include_directories(FS-404 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
