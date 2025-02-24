set(MiniBrowser_PRIVATE_INCLUDE_DIRECTORIES
    ${CMAKE_BINARY_DIR}
    ${WebCore_PRIVATE_FRAMEWORK_HEADERS_DIR}
)

set(MiniBrowser_SOURCES
    BrowserWindow.cpp
    Common.cpp
    MainWindow.cpp
    MiniBrowserLib.rc
    WinMain.cpp
    stdafx.cpp
)

set(MiniBrowser_PRIVATE_DEFINITIONS _UNICODE)
set(MiniBrowser_PRIVATE_LIBRARIES
    WebKit::WTF
    comctl32
    shlwapi
    user32
)

if (USE_CF)
    list(APPEND MiniBrowser_PRIVATE_LIBRARIES
        ${COREFOUNDATION_LIBRARY}
    )
endif ()

if (ENABLE_WEBKIT_LEGACY)
    list(APPEND MiniBrowser_PRIVATE_DEFINITIONS ENABLE_WEBKIT_LEGACY)
    list(APPEND MiniBrowser_INCLUDE_DIRECTORIES
        ${WebKitLegacy_FRAMEWORK_HEADERS_DIR}
    )
    list(APPEND MiniBrowser_SOURCES
        AccessibilityDelegate.cpp
        MiniBrowserWebHost.cpp
        PrintWebUIDelegate.cpp
        ResourceLoadDelegate.cpp
        WebDownloadDelegate.cpp
        WebKitLegacyBrowserWindow.cpp
    )
    list(APPEND MiniBrowser_PRIVATE_LIBRARIES
        WebKitLegacy
        WebKitLegacyGUID
    )
endif ()

if (ENABLE_WEBKIT)
    list(APPEND MiniBrowser_PRIVATE_DEFINITIONS ENABLE_WEBKIT)
    list(APPEND MiniBrowser_SOURCES
        WebKitBrowserWindow.cpp
    )
    list(APPEND MiniBrowser_PRIVATE_LIBRARIES
        WebKit::WebKit
    )
endif ()

WEBKIT_EXECUTABLE_DECLARE(MiniBrowser)
WEBKIT_WRAP_EXECUTABLE(MiniBrowser
    SOURCES ${TOOLS_DIR}/win/DLLLauncher/DLLLauncherMain.cpp MiniBrowser.rc
    LIBRARIES shlwapi
)
WEBKIT_EXECUTABLE(MiniBrowser)

set_target_properties(MiniBrowser PROPERTIES WIN32_EXECUTABLE ON)
if (${WTF_PLATFORM_WIN_CAIRO})
    target_compile_definitions(MiniBrowser PRIVATE WIN_CAIRO)
endif ()
