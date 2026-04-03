# ---------------------------------------------------------------------------
# CPack packaging configuration
# Included from CMakeLists.txt after install() targets are defined.
# ---------------------------------------------------------------------------

set(CPACK_PACKAGE_NAME              "stlviewer")
set(CPACK_PACKAGE_VENDOR            "Cravesoft")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "STL file viewer")
set(CPACK_PACKAGE_VERSION           "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR     "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR     "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH     "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT           "cravesoft@gmail.com")
set(CPACK_RESOURCE_FILE_LICENSE     "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

# ---------------------------------------------------------------------------
# Debian / Ubuntu (.deb)
# ---------------------------------------------------------------------------
set(CPACK_DEBIAN_PACKAGE_NAME           "stlviewer")
set(CPACK_DEBIAN_PACKAGE_SECTION        "graphics")
set(CPACK_DEBIAN_PACKAGE_PRIORITY       "optional")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE   "amd64")
set(CPACK_DEBIAN_PACKAGE_DEPENDS
    "libqt6widgets6 (>= 6.0), libqt6opengl6 (>= 6.0), libgl1")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER     "Olivier Crave <cravesoft@gmail.com>")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION
    "STLViewer\n A lightweight viewer for STL 3D model files.")

# ---------------------------------------------------------------------------
# Windows NSIS (.exe installer)
# ---------------------------------------------------------------------------
set(CPACK_NSIS_PACKAGE_NAME         "STLViewer")
set(CPACK_NSIS_DISPLAY_NAME         "STLViewer ${PROJECT_VERSION}")
set(CPACK_NSIS_INSTALL_ROOT         "$PROGRAMFILES64")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MUI_FINISHPAGE_RUN   "stlviewer.exe")
set(CPACK_NSIS_URL_INFO_ABOUT       "http://www.cravesoft.com")
set(CPACK_NSIS_CONTACT              "cravesoft@gmail.com")
set(CPACK_NSIS_MODIFY_PATH          OFF)
# Create Start Menu shortcut
set(CPACK_NSIS_CREATE_ICONS_EXTRA
    "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\STLViewer.lnk' '$INSTDIR\\\\bin\\\\stlviewer.exe'")
set(CPACK_NSIS_DELETE_ICONS_EXTRA
    "Delete '$SMPROGRAMS\\\\$START_MENU\\\\STLViewer.lnk'")

# ---------------------------------------------------------------------------
# Select generator per platform if not already set
# ---------------------------------------------------------------------------
if(NOT CPACK_GENERATOR)
    if(WIN32)
        set(CPACK_GENERATOR "NSIS")
    elseif(UNIX AND NOT APPLE)
        set(CPACK_GENERATOR "DEB")
    elseif(APPLE)
        set(CPACK_GENERATOR "DragNDrop")
    endif()
endif()

include(CPack)
