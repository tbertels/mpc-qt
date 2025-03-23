cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(mpc-qt CXX)
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS "-Wall -Wextra -O2")

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON) # For clangd

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Qt6 REQUIRED COMPONENTS Gui Network OpenGLWidgets Svg Widgets)
find_package(Qt6 REQUIRED COMPONENTS LinguistTools)
find_package(PkgConfig REQUIRED)

# Allow overriding version from the command line
if(DEFINED MPCQT_VERSION)
    message(STATUS "Version provided on the command line: ${MPCQT_VERSION}")
    set(VERSTR "${MPCQT_VERSION}")
    set(VERSTR_WIN "${VERSTR}.0")
else()
    # Check if we are in a git repository
    find_package(Git QUIET)
    if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
        message(STATUS "Deducing version from git information...")
        
        execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0
            OUTPUT_VARIABLE VERSTR
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-list ${VERSTR}..HEAD --count
            OUTPUT_VARIABLE VERSTR_BLD
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        message(STATUS "     ...the last tag is ${VERSTR}.")
        message(STATUS "     ...there are ${VERSTR_BLD} commits since the last tag.")

        string(REPLACE "v" "" VERSTR ${VERSTR})
        set(VERSTR_WIN "${VERSTR}.${VERSTR_BLD}")

        if(NOT "${VERSTR_BLD}" STREQUAL "0")
            message(STATUS "     ...this is probably a beta build, using descriptive version information.")
            execute_process(
                COMMAND ${GIT_EXECUTABLE} describe --tags
                OUTPUT_VARIABLE VERSTR
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            string(REPLACE "v" "" VERSTR ${VERSTR})
            add_definitions(-DMPCQT_DEVELOPMENT)
        endif()
    else()
        message(STATUS "Not in a git repo and no version information provided.")
        message(STATUS "This will appear as an unversioned development build.")
        message(STATUS "To pass a version to CMake, run it like this:")
        message(STATUS "    cmake -DMPCQT_VERSION=<version> ..")
        set(VERSTR "Unspecified_version")
        set(VERSTR_WIN "0.0.0.0")
    endif()
endif()

# Define version as a preprocessor definition
add_definitions(-DMPCQT_VERSION_STR=\"${VERSTR}\")

message(STATUS "The version appears to be ${VERSTR}, and the manifest would say this is ${VERSTR_WIN}.")

# Windows-specific metadata
if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    set(QMAKE_TARGET_COMPANY "The Mpc-Qt developers")
    set(QMAKE_TARGET_COPYRIGHT "Copyright (c) 2015 The Mpc-Qt developers")
    set(QMAKE_TARGET_PRODUCT "Media Player Classic Qute Theater")
    set(QMAKE_TARGET_DESCRIPTION "MPC-QT")
    set(PROJECT_VERSION ${VERSTR_WIN})
endif()


set(QM_FILES_RESOURCE_PREFIX "/i18n/")
set(TS_FILES
    translations/mpc-qt_ca.ts
    translations/mpc-qt_en.ts
    translations/mpc-qt_es.ts
    translations/mpc-qt_fi.ts
    translations/mpc-qt_id.ts
    translations/mpc-qt_it.ts
    translations/mpc-qt_nl.ts
    translations/mpc-qt_ru.ts
    translations/mpc-qt_ta.ts
    translations/mpc-qt_tr.ts
    translations/mpc-qt_zh_CN.ts
)

# Generate QM files
set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION ".qm")
qt_add_translations(mpc-qt
    TS_FILES ${TS_FILES}
    RESOURCE_PREFIX ${QM_FILES_RESOURCE_PREFIX}
)

qt_standard_project_setup()

qt_add_executable(mpc-qt WIN32 MACOSX_BUNDLE
    favoriteswindow.cpp favoriteswindow.h favoriteswindow.ui
    gotowindow.cpp gotowindow.h gotowindow.ui
    helpers.cpp helpers.h
    ipc/http.cpp ipc/http.h
    ipc/json.cpp ipc/json.h
    librarywindow.cpp librarywindow.h librarywindow.ui
    logger.cpp logger.h
    logwindow.cpp logwindow.h logwindow.ui
    main.cpp main.h
    mainwindow.cpp mainwindow.h mainwindow.ui
    manager.cpp manager.h
    mpvwidget.cpp mpvwidget.h
    openfiledialog.cpp openfiledialog.h openfiledialog.ui
    platform/devicemanager.cpp platform/devicemanager.h
    platform/screensaver.cpp platform/screensaver.h
    platform/unify.cpp platform/unify.h
    platform/windowmanager.cpp platform/windowmanager.h
    playlist.cpp playlist.h
    playlistwindow.cpp playlistwindow.h playlistwindow.ui
    propertieswindow.cpp propertieswindow.h propertieswindow.ui
    settingswindow.cpp settingswindow.h settingswindow.ui
    storage.cpp storage.h
    thumbnailerwindow.cpp thumbnailerwindow.h thumbnailerwindow.ui
    widgets/actioneditor.cpp widgets/actioneditor.h
    widgets/drawncollection.cpp widgets/drawncollection.h
    widgets/drawnplaylist.cpp widgets/drawnplaylist.h
    widgets/drawnslider.cpp widgets/drawnslider.h
    widgets/drawnstatus.cpp widgets/drawnstatus.h
    widgets/logowidget.cpp widgets/logowidget.h
    widgets/paletteeditor.cpp widgets/paletteeditor.h
    widgets/screencombo.cpp widgets/screencombo.h
)

target_link_libraries(mpc-qt PRIVATE
    Qt::Gui
    Qt::Network
    Qt::OpenGLWidgets
    Qt::Svg
    Qt::Widgets
)

if(UNIX)
    target_sources(mpc-qt PUBLIC
        ipc/mpris.cpp ipc/mpris.h
        platform/devicemanager_unix.cpp platform/devicemanager_unix.h
        platform/screensaver_unix.cpp platform/screensaver_unix.h
    )

    target_link_libraries(mpc-qt PRIVATE
        Qt::DBus
    )
endif()

# if(((NOT (NOT MPCQT_VERSION_ISEMPTY)) AND (EXISTS .git)) AND (NOT VERSTR_BLD___equals___0))
#     target_compile_definitions(mpc-qt PRIVATE
#         MPCQT_DEVELOPMENT
#     )
# endif()

if(WIN32)
    target_sources(mpc-qt PUBLIC
        platform/devicemanager_win.cpp platform/devicemanager_win.h
        platform/screensaver_win.cpp platform/screensaver_win.h
    )

    target_link_libraries(mpc-qt PRIVATE
        powrprof
    )
endif()

# if(WIN32 AND NOT ENABLE_LOCAL_MPV_ISEMPTY)
#     target_include_directories(mpc-qt PRIVATE
#         mpv-dev/include
#     )

#     target_link_libraries(mpc-qt PRIVATE
#         # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/mpv-dev/lib/
#         libmpv
#     )
# endif()


# Unix-specific settings
if(UNIX)
    # find_package(Qt6 REQUIRED COMPONENTS DBus)
    # target_link_libraries(mpc-qt PRIVATE Qt6::DBus)# FIXME duplicate

    # Use pkg-config to find MPV
    pkg_check_modules(MPV REQUIRED mpv)

    target_include_directories(mpc-qt PRIVATE ${MPV_INCLUDE_DIRS})
    target_link_libraries(mpc-qt PRIVATE ${MPV_LIBRARIES})
    add_compile_definitions(${MPV_CFLAGS_OTHER})
endif()

# Windows-specific settings
if(WIN32)
    target_link_libraries(mpc-qt PRIVATE powrprof)

    if(DEFINED ENABLE_LOCAL_MPV AND NOT ENABLE_LOCAL_MPV STREQUAL "")
        target_include_directories(mpc-qt PRIVATE ${CMAKE_SOURCE_DIR}/mpv-dev/include)
        target_link_directories(mpc-qt PRIVATE ${CMAKE_SOURCE_DIR}/mpv-dev/lib)
        target_link_libraries(mpc-qt PRIVATE mpv)
    else()
        pkg_check_modules(MPV REQUIRED mpv)

        target_include_directories(mpc-qt PRIVATE ${MPV_INCLUDE_DIRS})
        target_link_libraries(mpc-qt PRIVATE ${MPV_LIBRARIES})
        add_compile_definitions(${MPV_CFLAGS_OTHER})
    endif()
endif()


set(res_resource_files
    "http/browser.html"
    "http/index.html"
    "http/info.html"
    "http/variables.html"
    "images/icon/mpc-qt.svg"
    "images/icon/tinyicon.svg"
    "images/logo/cinema-screen.svg"
    "images/logo/mpv-vlc.svg"
    "images/logo/triangle-circle.svg"
    "images/theme/black/document-export.svg"
    "images/theme/black/document-import.svg"
    "images/theme/black/go-next.svg"
    "images/theme/black/go-previous.svg"
    "images/theme/black/media-playback-pause.svg"
    "images/theme/black/media-playback-start.svg"
    "images/theme/black/media-playback-stop.svg"
    "images/theme/black/media-queue-visible.svg"
    "images/theme/black/media-seek-backward.svg"
    "images/theme/black/media-seek-forward.svg"
    "images/theme/black/media-skip-backward.svg"
    "images/theme/black/media-skip-forward.svg"
    "images/theme/black/player-volume-muted.svg"
    "images/theme/black/player-volume.svg"
    "images/theme/black/tab-close.svg"
    "images/theme/black/tab-duplicate.svg"
    "images/theme/black/tab-new.svg"
    "images/theme/black/view-media-queue.svg"
    "images/theme/black/view-media-subtitles-hidden.svg"
    "images/theme/black/view-media-subtitles.svg"
    "images/theme/black/zone-in.svg"
    "images/theme/black/zone-out.svg"
    "images/theme/white/document-export.svg"
    "images/theme/white/document-import.svg"
    "images/theme/white/go-next.svg"
    "images/theme/white/go-previous.svg"
    "images/theme/white/media-playback-pause.svg"
    "images/theme/white/media-playback-start.svg"
    "images/theme/white/media-playback-stop.svg"
    "images/theme/white/media-queue-visible.svg"
    "images/theme/white/media-seek-backward.svg"
    "images/theme/white/media-seek-forward.svg"
    "images/theme/white/media-skip-backward.svg"
    "images/theme/white/media-skip-forward.svg"
    "images/theme/white/player-volume-muted.svg"
    "images/theme/white/player-volume.svg"
    "images/theme/white/tab-close.svg"
    "images/theme/white/tab-duplicate.svg"
    "images/theme/white/tab-new.svg"
    "images/theme/white/video-x-generic-16.svg"
    "images/theme/white/video-x-generic-32.svg"
    "images/theme/white/view-media-queue.svg"
    "images/theme/white/view-media-subtitles-hidden.svg"
    "images/theme/white/view-media-subtitles.svg"
    "images/theme/white/zone-in.svg"
    "images/theme/white/zone-out.svg"
    "text/encodeFormat.html"
    "text/playlistFormat.html"
)

qt_add_resources(mpc-qt "res"
    PREFIX
        "/"
    FILES
        ${res_resource_files}
)

if(UNIX)
    if(NOT DEFINED PREFIX)
        set(PREFIX "/usr/local")
    endif()

    target_compile_definitions(mpc-qt PRIVATE MPCQT_PREFIX="${PREFIX}")

    install(TARGETS mpc-qt DESTINATION ${PREFIX}/bin)
    install(FILES DOCS/ipc.md DESTINATION ${PREFIX}/share/doc/mpc-qt)
    install(FILES io.github.mpc-qt.mpc-qt.desktop DESTINATION ${PREFIX}/share/applications)
    install(FILES images/icon/mpc-qt.svg DESTINATION ${PREFIX}/share/icons/hicolor/scalable/apps)
endif()
